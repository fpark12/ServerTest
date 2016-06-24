#include "ParamBind.h"

ParamBind::ParamBind(std::vector<QueryParamSizeInfo>&& paramSizeInfoList) :
	ParamCount(paramSizeInfoList.size()), ParamBuffer(nullptr),
	MySqlBinds(new MYSQL_BIND[ParamCount]), ParamsCppType(ParamCount),
	bValueSet(ParamCount, true),	TotalParamSize(0)
{
	ASSERT(ParamCount <= MaxParamCount);
	sumOfSize(paramSizeInfoList);
	ParamBuffer = new uint8[TotalParamSize];
	int bufferOffset = 0;
	for (std::size_t i = 0;
		i < paramSizeInfoList.size() && bufferOffset < TotalParamSize;
		++i)
	{
		MySqlBinds[i].buffer = (void*)(ParamBuffer + bufferOffset);
		SetMySqlBindTypeInfo(paramSizeInfoList[i], MySqlBinds[i]);
		ParamsCppType[i] = paramSizeInfoList[i].Type;
		if (paramSizeInfoList[i].Type == TYPE_STRING ||
			paramSizeInfoList[i].Type == TYPE_BINARY)
		{
			bufferOffset += paramSizeInfoList[i].MaxDataLength;
		}
		else
		{
			bufferOffset += ParamSize(paramSizeInfoList[i].Type);
		}
	}
}

ParamBind::~ParamBind()
{
	delete[] ParamBuffer;
	ParamBuffer = nullptr;
	delete[] MySqlBinds;
	MySqlBinds = nullptr;
}

void ParamBind::ClearParameters()
{
	memset(ParamBuffer, NULL, TotalParamSize);
	for (auto binded : bValueSet)
	{
		binded = false;
	}
}

QueryParamCPPType ParamBind::GetParamType(uint8 index)
{
	return ParamsCppType[index];
}

unsigned long ParamBind::GetStringParamMaxLength(uint8 index)
{
	if (ParamsCppType[index] == TYPE_STRING
		|| ParamsCppType[index] == TYPE_BINARY)
	{
		return MySqlBinds[index].buffer_length;
	}
	else
	{
		return 0;
	}
}

std::size_t ParamBind::sumOfSize(std::vector<QueryParamSizeInfo>& paramSizeInfoList)
{
	for (QueryParamSizeInfo& paramSizeInfo : paramSizeInfoList)
	{
		if (paramSizeInfo.Type == TYPE_STRING || paramSizeInfo.Type == TYPE_BINARY)
		{
			TotalParamSize += paramSizeInfo.MaxDataLength;
		}
		else
		{
			TotalParamSize += ParamSize(paramSizeInfo.Type);
		}
	}
	return TotalParamSize;
}

int32 ParamBind::ParamSize(QueryParamCPPType Type)
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

void ParamBind::SetMySqlBindTypeInfo(QueryParamSizeInfo& paramSizeInfo, MYSQL_BIND& bind)
{
	switch (paramSizeInfo.Type)
	{
	case TYPE_BOOL:
	case TYPE_UI8:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_TINY;
		bind.buffer_length = sizeof(uint8);
		bind.is_unsigned = false;
		break;
	case TYPE_UI16:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_SHORT;
		bind.buffer_length = sizeof(uint16);
		bind.is_unsigned = false;
		break;
	case TYPE_UI32:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_LONG;
		bind.buffer_length = sizeof(uint32);
		bind.is_unsigned = false;
		break;
	case TYPE_UI64:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_LONGLONG;
		bind.buffer_length = sizeof(uint64);
		bind.is_unsigned = false;
		break;
	case TYPE_I8:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_TINY;
		bind.buffer_length = sizeof(int8);
		bind.is_unsigned = true;
		break;
	case TYPE_I16:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_SHORT;
		bind.buffer_length = sizeof(int16);
		bind.is_unsigned = true;
		break;
	case TYPE_I32:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_LONG;
		bind.buffer_length = sizeof(int32);
		bind.is_unsigned = true;
		break;
	case TYPE_I64:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_LONGLONG;
		bind.buffer_length = sizeof(int64);
		bind.is_unsigned = true;
		break;
	case TYPE_FLOAT:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_FLOAT;
		bind.buffer_length = sizeof(float);
		bind.is_unsigned = false;
		break;
	case TYPE_DOUBLE:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_DOUBLE;
		bind.buffer_length = sizeof(double);
		bind.is_unsigned = false;
		break;
	case TYPE_STRING:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_VAR_STRING;
		bind.buffer_length = paramSizeInfo.MaxDataLength;
		bind.is_null_value = 0;
		break;
	case TYPE_BINARY:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_BLOB;
		bind.buffer_length = paramSizeInfo.MaxDataLength;
		bind.is_null_value = 0;
		break;
	case TYPE_NULL:
		bind.buffer_type = enum_field_types::MYSQL_TYPE_NULL;
		delete[] static_cast<char *>(bind.buffer);
		bind.buffer = nullptr;
		bind.buffer_length = 0;
		bind.is_null_value = 1;
		delete bind.length;
		bind.length = nullptr;
		break;
	default:
		break;
	}
}
