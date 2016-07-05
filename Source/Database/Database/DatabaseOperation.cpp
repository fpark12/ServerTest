#include "DatabaseOperation.h"

DatabaseOperation::DatabaseOperation() :
	MySqlStatementHandle(nullptr),
	OperationFlag(SqlOperationFlag::Neither),
	ParamCount(0),
	ParamSetMask(0x00000000),
	ParamBindHandle(nullptr),
	ParamDataSerialization(nullptr),
	Storage(StatementStorage::None)
{
}

DatabaseOperation::~DatabaseOperation()
{
	ClearParam();
}

void DatabaseOperation::ClearParam()
{
	if (ParamCount)
	{
		delete[] ParamDataSerialization;
	}

	if (ParamBindHandle)
	{
		for (uint32 i = 0; i < ParamCount; ++i)
		{
			if (ParamBindHandle[i].buffer_type == MYSQL_TYPE_VAR_STRING ||
				ParamBindHandle[i].buffer_type == MYSQL_TYPE_BLOB)
			{
				delete ParamBindHandle[i].buffer;
			}
			delete ParamBindHandle[i].is_null;
			delete ParamBindHandle[i].length;
		}
	}

	delete[] ParamBindHandle;
}

void DatabaseOperation::SetStatement(MYSQL_STMT* stmt)
{
	MySqlStatementHandle = stmt;
	ParamCount = mysql_stmt_param_count(stmt);
	ParamSetMask = 0x00000000;
	ParamBindHandle = new MYSQL_BIND[ParamCount];
	memset(ParamBindHandle, 0, sizeof(MYSQL_BIND)*ParamCount);
	ParamDataSerialization = new uint64[ParamCount];
	memset(ParamDataSerialization, 0, sizeof(uint64)*ParamCount);

	/// "If set to 1, causes mysql_stmt_store_result() to update the metadata MYSQL_FIELD->max_length value."
	my_bool bool_tmp = 1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
}

void DatabaseOperation::SetOperationFlag(SqlOperationFlag flag)
{
	OperationFlag = flag;
}

void DatabaseOperation::BindParam()
{
	mysql_stmt_bind_param(MySqlStatementHandle, ParamBindHandle);
}

void DatabaseOperation::SetParamBool(uint8 index, bool&& value)
{
	SetParamUInt8(index, uint8(value));
}

void DatabaseOperation::SetParamUInt8(uint8 index, uint8&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_TINY, &ParamDataSerialization[index], true);
}

void DatabaseOperation::SetParamInt8(uint8 index, int8&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_TINY, &ParamDataSerialization[index], false);
}

void DatabaseOperation::SetParamUInt16(uint8 index, uint16&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_SHORT, &ParamDataSerialization[index], true);
}

void DatabaseOperation::SetParamInt16(uint8 index, int16&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_SHORT, &ParamDataSerialization[index], false);
}

void DatabaseOperation::SetParamUInt32(uint8 index, uint32&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_LONG, &ParamDataSerialization[index], true);
}

void DatabaseOperation::SetParamInt32(uint8 index, int32&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_LONG, &ParamDataSerialization[index], false);
}

void DatabaseOperation::SetParamUInt64(uint8 index, uint64&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_LONGLONG, &ParamDataSerialization[index], true);
}

void DatabaseOperation::SetParamInt64(uint8 index, int64&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_LONGLONG, &ParamDataSerialization[index], false);
}

void DatabaseOperation::SetParamFloat(uint8 index, float&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_FLOAT, &ParamDataSerialization[index], (value > 0.0f));
}

void DatabaseOperation::SetParamDouble(uint8 index, double&& value)
{
	ParamSetMask ^= 0x00000001 << index;
	ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_DOUBLE, &ParamDataSerialization[index], (value > 0.0f));
}

void DatabaseOperation::SetParamString(uint8 index, char const* value)
{
	ParamSetMask ^= 0x00000001 << index;

	uint32 len = uint32(strlen(value));
	char* stringLocation = new char[len];
	memcpy(stringLocation, value, len);

	ParamDataSerialization[index] = reinterpret_cast<uint64&>(stringLocation);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_VAR_STRING, stringLocation, false, len, len - 1);
}

void DatabaseOperation::SetParamString(uint8 index, std::string&& value)
{
	ParamSetMask ^= 0x00000001 << index;

	uint32 len = value.size() + 1;
	char* stringLocation = new char[len];
	memcpy(stringLocation, value.c_str(), len);

	ParamDataSerialization[index] = reinterpret_cast<uint64&>(stringLocation);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_VAR_STRING, stringLocation, false, len, len - 1);
}

void DatabaseOperation::SetParamBinary(uint8 index, const void* value, uint32 dataSize)
{
	ParamSetMask ^= 0x00000001 << index;

	char* binaryLocation = new char[dataSize];
	memcpy(binaryLocation, value, dataSize);

	ParamDataSerialization[index] = reinterpret_cast<uint64&>(binaryLocation);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_BLOB, binaryLocation, false, dataSize, dataSize);
}

void DatabaseOperation::SetParamBinary(uint8 index, std::vector<uint8>&& value)
{
	ParamSetMask ^= 0x00000001 << index;

	uint32 dataSize = value.size();
	char* binaryLocation = new char[dataSize];
	memcpy(binaryLocation, value.data(), dataSize);

	ParamDataSerialization[index] = reinterpret_cast<uint64&>(binaryLocation);
	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_BLOB, binaryLocation, false, dataSize, dataSize);
}

void DatabaseOperation::SetParamNull(uint8 index)
{
	ParamSetMask ^= 0x00000001 << index;

	SetMySqlBind(&ParamBindHandle[index], MYSQL_TYPE_NULL, NULL, false, 0, 0, true);
}

void DatabaseOperation::SetMySqlBind(MYSQL_BIND* mySqlBind, enum_field_types bufferType, void* bufferLocation, bool isUnsigned, uint32 bufferLength /*= 0*/, uint32 dataSize /*= 0*/, bool isNull /*= 0*/)
{
	mySqlBind->buffer_type = bufferType;
	mySqlBind->buffer = bufferLocation;
	mySqlBind->buffer_length = bufferLength;
	mySqlBind->is_null_value = isNull;
	if (dataSize > 0)
	{
		if (mySqlBind->length)
		{
			*(mySqlBind->length) = dataSize;
		}
		else
		{
			mySqlBind->length = new unsigned long(dataSize);
		}
	}
	/*
	else
	{
		delete mySqlBind->length;
		mySqlBind->length = nullptr;
	}
	*/
	mySqlBind->is_unsigned = isUnsigned;
}
