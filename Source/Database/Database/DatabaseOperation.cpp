#include "DatabaseOperation.h"

DatabaseOperation::DatabaseOperation(std::vector<QueryParamCPPType>&& paramTypeList) :
	ParamCount(paramTypeList.size()),
	TotalParamSize(ParamsTotalSize(paramTypeList)),
	ParamBuffer(new uint8[TotalParamSize]),
	MySqlBinds(new MySqlBindWrapper[ParamCount]),
	ParamsCppType(ParamCount),
	bValueSet(ParamCount, false)
{
	ASSERT(ParamCount <= MaxParamCount);
	unsigned long bufferOffset = 0;
	for (std::size_t i = 0;
		i < ParamCount && bufferOffset < TotalParamSize;
		++i)
	{
		MySqlBinds[i].Bind.buffer = (void*)(ParamBuffer + bufferOffset);
		SetMySqlBindTypeInfo(paramTypeList[i], MySqlBinds[i]);
		ParamsCppType[i] = paramTypeList[i].Type;
		if (paramTypeList[i].Type == TYPE_STRING ||
			paramTypeList[i].Type == TYPE_BINARY)
		{
			bufferOffset += paramTypeList[i].MaxDataLength;
		}
		else
		{
			bufferOffset += ParamSize(paramTypeList[i].Type);
		}
	}
}

DatabaseOperation::DatabaseOperation(MYSQL_FIELD* resultFields, uint8 fieldCount,
	uint8* buffer, std::size_t bufferLength, MySqlBindWrapper* bindWrapperBuffer) :
	ParamCount(fieldCount),
	TotalParamSize(bufferLength),
	ParamBuffer(buffer),
	MySqlBinds(bindWrapperBuffer),
	bValueSet(ParamCount, false)
{
	ASSERT(ParamCount <= MaxParamCount);
	unsigned long bufferOffset = 0;
	for (std::size_t i = 0;
		i < ParamCount && bufferOffset < TotalParamSize;
		++i)
	{
		MySqlBinds[i].Bind.buffer_type = resultFields[i].type;
		MySqlBinds[i].Bind.buffer_length = ParamSize(resultFields[i]);
		MySqlBinds[i].Bind.length = &MySqlBinds[i].FieldLength;
		MySqlBinds[i].Bind.is_null = &MySqlBinds[i].NullIndicator;
		MySqlBinds[i].Bind.error = NULL;
		MySqlBinds[i].Bind.is_unsigned = resultFields[i].flags & UNSIGNED_FLAG;
		MySqlBinds[i].Bind.buffer = (void*)(ParamBuffer + bufferOffset);
		bufferOffset += MySqlBinds[i].Bind.buffer_length;
	}
}

DatabaseOperation::~DatabaseOperation()
{
	delete[] ParamBuffer;
	ParamBuffer = nullptr;
	delete[] MySqlBinds;
	MySqlBinds = nullptr;
}

void DatabaseOperation::ClearParameters()
{
	memset(ParamBuffer, NULL, TotalParamSize);
	for (auto binded : bValueSet)
	{
		binded = false;
	}
}

QueryParamCPPType DatabaseOperation::GetParamType(uint8 index)
{
	return ParamsCppType[index];
}

unsigned long DatabaseOperation::GetStringParamMaxLength(uint8 index)
{
	if (ParamsCppType[index] == TYPE_STRING
		|| ParamsCppType[index] == TYPE_BINARY)
	{
		return MySqlBinds[index].Bind.buffer_length;
	}
	else
	{
		return 0;
	}
}

void DatabaseOperation::SetMySqlBindTypeInfo(QueryParamDetails& paramSizeInfo, MySqlBindWrapper& bindWrapper)
{
	switch (paramSizeInfo.Type)
	{
	case TYPE_BOOL:
	case TYPE_UI8:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_TINY;
		bindWrapper.Bind.buffer_length = sizeof(uint8);
		bindWrapper.Bind.is_unsigned = false;
		break;
	case TYPE_UI16:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_SHORT;
		bindWrapper.Bind.buffer_length = sizeof(uint16);
		bindWrapper.Bind.is_unsigned = false;
		break;
	case TYPE_UI32:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_LONG;
		bindWrapper.Bind.buffer_length = sizeof(uint32);
		bindWrapper.Bind.is_unsigned = false;
		break;
	case TYPE_UI64:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_LONGLONG;
		bindWrapper.Bind.buffer_length = sizeof(uint64);
		bindWrapper.Bind.is_unsigned = false;
		break;
	case TYPE_I8:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_TINY;
		bindWrapper.Bind.buffer_length = sizeof(int8);
		bindWrapper.Bind.is_unsigned = true;
		break;
	case TYPE_I16:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_SHORT;
		bindWrapper.Bind.buffer_length = sizeof(int16);
		bindWrapper.Bind.is_unsigned = true;
		break;
	case TYPE_I32:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_LONG;
		bindWrapper.Bind.buffer_length = sizeof(int32);
		bindWrapper.Bind.is_unsigned = true;
		break;
	case TYPE_I64:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_LONGLONG;
		bindWrapper.Bind.buffer_length = sizeof(int64);
		bindWrapper.Bind.is_unsigned = true;
		break;
	case TYPE_FLOAT:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_FLOAT;
		bindWrapper.Bind.buffer_length = sizeof(float);
		bindWrapper.Bind.is_unsigned = false;
		break;
	case TYPE_DOUBLE:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_DOUBLE;
		bindWrapper.Bind.buffer_length = sizeof(double);
		bindWrapper.Bind.is_unsigned = false;
		break;
	case TYPE_STRING:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_VAR_STRING;
		bindWrapper.Bind.buffer_length = paramSizeInfo.MaxDataLength;
		bindWrapper.Bind.is_null_value = 0;
		break;
	case TYPE_BINARY:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_BLOB;
		bindWrapper.Bind.buffer_length = paramSizeInfo.MaxDataLength;
		bindWrapper.Bind.is_null_value = 0;
		break;
	case TYPE_NULL:
		bindWrapper.Bind.buffer_type = enum_field_types::MYSQL_TYPE_NULL;
		delete[] static_cast<char *>(bindWrapper.Bind.buffer);
		bindWrapper.Bind.buffer = nullptr;
		bindWrapper.Bind.buffer_length = 0;
		bindWrapper.Bind.is_null_value = 1;
		delete bindWrapper.Bind.length;
		bindWrapper.Bind.length = nullptr;
		break;
	default:
		break;
	}
}

std::size_t DatabaseOperation::ParamsTotalSize(std::vector<QueryParamDetails>& paramSizeInfoList)
{
	std::size_t totalSize = 0;
	for (QueryParamDetails& paramSizeInfo : paramSizeInfoList)
	{
		if (paramSizeInfo.Type == TYPE_STRING || paramSizeInfo.Type == TYPE_BINARY)
		{
			totalSize += paramSizeInfo.MaxDataLength;
		}
		else
		{
			totalSize += ParamSize(paramSizeInfo.Type);
		}
	}
	return totalSize;
}

std::size_t DatabaseOperation::ParamsTotalSize(MYSQL_FIELD* fields, uint8 fieldCount)
{
	std::size_t totalSize = 0;
	for (uint8 i = 0; i < fieldCount; ++i)
	{
		totalSize += ParamSize(fields[i]);
	}
	return totalSize;
}

std::size_t DatabaseOperation::ParamSize(QueryParamCPPType Type)
{
	switch (Type)
	{
	case TYPE_BOOL:
		return sizeof(bool);
	case TYPE_UI8:
		return sizeof(uint8);
	case TYPE_UI16:
		return sizeof(uint16);
	case TYPE_UI32:
		return sizeof(uint32);
	case TYPE_UI64:
		return sizeof(uint64);
	case TYPE_I8:
		return sizeof(int8);
	case TYPE_I16:
		return sizeof(int16);
	case TYPE_I32:
		return sizeof(int32);
	case TYPE_I64:
		return sizeof(int64);
	case TYPE_FLOAT:
		return sizeof(float);
	case TYPE_DOUBLE:
		return sizeof(double);
	case TYPE_STRING:
	case TYPE_BINARY:
		return -1;
	case TYPE_NULL:
		return 0;
	default:
		//TODO error log system
		throw std::invalid_argument("Received invalid param type");
	}
}

std::size_t DatabaseOperation::ParamSize(MYSQL_FIELD& field)
{
	switch (field.type)
	{
	case MYSQL_TYPE_NULL:
		return 0;
	case MYSQL_TYPE_TINY:
		return 1;
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_SHORT:
		return 2;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
	case MYSQL_TYPE_FLOAT:
		return 4;
	case MYSQL_TYPE_DOUBLE:
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_BIT:
		return 8;

	case MYSQL_TYPE_TIMESTAMP:
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_DATETIME:
		return sizeof(MYSQL_TIME);

	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
		return field.max_length + 1;

	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_NEWDECIMAL:
		return 64;

	case MYSQL_TYPE_GEOMETRY:
		/*
		Following types are not sent over the wire:
		MYSQL_TYPE_ENUM:
		MYSQL_TYPE_SET:
		*/
	default:
		// TC_LOG_WARN("sql.sql", "SQL::SizeForType(): invalid field type %u", uint32(field->type));
		return 0;
	}
}
