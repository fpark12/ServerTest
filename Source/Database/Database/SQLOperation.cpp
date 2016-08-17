#include "SQLOperation.h"
#include "DatabaseConnection.h"

SQLOperation::SQLOperation(DatabaseConnection* conn) :
	SqlConnection(conn),
	SqlStatement(nullptr),
	OperationFlag(SqlOperationFlag::Neither),
	ParamCount(0),
	ParamSetBitMask(0x00000000),
	ParamBinds(nullptr),
	ParamData(nullptr),
	Storage(StatementStorage::None),
	FieldBinds(nullptr),
	RowCount(0),
	FieldCount(0),
	CurrentRowCursor(0),
	RowData(nullptr),
	ResultSetData(nullptr),
	SQLOperationResult(SQLOperationResult::None)
{
}

SQLOperation::~SQLOperation()
{
	ClearParam();
	ClearResult();
}

void SQLOperation::ClearParam()
{
	if (ParamCount)
	{
		delete[] ParamData;
		ParamData = nullptr;
	}

	if (ParamBinds)
	{
		for (uint32 i = 0; i < ParamCount; ++i)
		{
			if (ParamBinds[i].buffer_type == MYSQL_TYPE_VAR_STRING ||
				ParamBinds[i].buffer_type == MYSQL_TYPE_BLOB)
			{
				delete[] ParamBinds[i].buffer;
			}
			delete ParamBinds[i].is_null;
			delete ParamBinds[i].length;
		}
	}

	delete[] ParamBinds;
	ParamBinds = nullptr;
}

void SQLOperation::ClearResult()
{
	if (FieldCount)
	{
		std::vector<uint32> extraMemoryItemIndex;

		for (uint32 i = 0; i < FieldCount; ++i)
		{
			// TODO mysql variable length type
			if (FieldBinds[i].buffer_type == MYSQL_TYPE_BLOB ||
				FieldBinds[i].buffer_type == MYSQL_TYPE_VAR_STRING)
			{
				extraMemoryItemIndex.push_back(i);
				delete[] FieldBinds[i].buffer;
			}
			delete FieldBinds[i].is_null;
			delete FieldBinds[i].length;
		}

		delete[] FieldBinds;
		FieldBinds = nullptr;

		for (uint64 row = 0; row < RowCount; ++row)
		{
			uint64* newRow = ResultSetData + FieldCount*row;
			for (auto& index : extraMemoryItemIndex)
			{
				delete[] reinterpret_cast<void*>(*(newRow + index));
			}
		}

		delete[] ResultSetData;
		ResultSetData = nullptr;
		delete[] RowData;
		RowData = nullptr;
	}
}

void SQLOperation::SetConnection(DatabaseConnection* conn)
{
	SqlConnection = conn;
}

void SQLOperation::SetStatement(MYSQL_STMT* stmt)
{
	SqlStatement = stmt;
	ParamCount = mysql_stmt_param_count(stmt);
	ParamBinds = new MYSQL_BIND[ParamCount];
	memset(ParamBinds, 0, sizeof(MYSQL_BIND)*ParamCount);
	ParamData = new uint64[ParamCount];
	memset(ParamData, 0, sizeof(uint64)*ParamCount);

	FieldCount = mysql_stmt_field_count(stmt);

	/// "If set to 1, causes mysql_stmt_store_result() to update the metadata MYSQL_FIELD->max_length value."
	my_bool bool_tmp = 1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
}

void SQLOperation::SetStatement(char* sql)
{
	MYSQL_STMT* stmt = SqlConnection->MySqlStatementHandle;

	if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
	{
		//TODO error log
		exit(EXIT_FAILURE);
	}

	SetStatement(stmt);
}

void SQLOperation::SetOperationFlag(SqlOperationFlag flag)
{
	OperationFlag = flag;
}

void SQLOperation::Execute()
{
	if (ParamCount)
	{
		mysql_stmt_bind_param(SqlStatement, ParamBinds);
	}

	if (mysql_stmt_execute(SqlStatement))
	{
		//TODO error log
		fprintf(stderr, " mysql_stmt_execute(), failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(SqlStatement));
		exit(EXIT_FAILURE);
	}

	if (FieldCount)
	{
		// Init data serialization
		FieldBinds = new MYSQL_BIND[FieldCount];
		memset(FieldBinds, 0, sizeof(MYSQL_BIND)*FieldCount);
		RowData = new uint64[FieldCount];
		memset(RowData, 0, sizeof(uint64)*FieldCount);

		if (mysql_stmt_store_result(SqlStatement))
		{
			//TODO error log
			fprintf(stderr, " mysql_stmt_execute(), failed\n");
			fprintf(stderr, " %s\n", mysql_stmt_error(SqlStatement));
			exit(EXIT_FAILURE);
		}

		// get metadata
		MYSQL_RES* resultMetaData = mysql_stmt_result_metadata(SqlStatement);
		MYSQL_FIELD* resultDataFields = mysql_fetch_fields(resultMetaData);
		RowCount = (uint64)mysql_stmt_num_rows(SqlStatement);

		// bind result for fetching
		if (SqlStatement->bind_result_done)
		{
			delete[] SqlStatement->bind->length;
			delete[] SqlStatement->bind->is_null;
		}

		for (uint32 i = 0; i < FieldCount; ++i)
		{

			uint32 size = SizeForType(&resultDataFields[i]);

			if (resultDataFields[i].type == MYSQL_TYPE_VAR_STRING ||
				resultDataFields[i].type == MYSQL_TYPE_BLOB)
			{
				char* fieldBuffer = new char[size];
				RowData[i] = reinterpret_cast<uint64&>(fieldBuffer);
				SetMySqlBind(&FieldBinds[i], resultDataFields[i].type,
					fieldBuffer, false, size, size);
			}
			else
			{
				SetMySqlBind(&FieldBinds[i], resultDataFields[i].type,
					&RowData[i], resultDataFields[i].flags & UNSIGNED_FLAG);
			}
		}

		mysql_stmt_bind_result(SqlStatement, FieldBinds);
		ResultSetData = new uint64[RowCount*FieldCount];

		uint32 rowIndex = 0;
		while (FetchNextRow())
		{
			for (uint32 fieldIndex = 0; fieldIndex < FieldCount; ++fieldIndex)
			{
				if (FieldBinds[fieldIndex].buffer_type == MYSQL_TYPE_BLOB ||
					FieldBinds[fieldIndex].buffer_type == MYSQL_TYPE_VAR_STRING)
				{
					uint32 fieldBufferSize = FieldBinds[fieldIndex].buffer_length;
					char* fieldBuffer = new char[fieldBufferSize];
					memcpy(fieldBuffer, (void*)RowData[fieldIndex], fieldBufferSize);
					ResultSetData[rowIndex*FieldCount + fieldIndex] = reinterpret_cast<uint64&>(fieldBuffer);
				}
				else
				{
					memcpy(&ResultSetData[rowIndex*FieldCount + fieldIndex],
						&RowData[fieldIndex], sizeof(uint64));
				}
			}
			++rowIndex;
		}
		mysql_stmt_free_result(SqlStatement);
	}
}

void SQLOperation::Call()
{
	Execute();
	SQLOperationResult = SQLOperationResult::Success;
}

void SQLOperation::SetParamBool(uint8 index, const bool& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_TINY, &ParamData[index], true);
}

void SQLOperation::SetParamUInt8(uint8 index, const uint8& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_TINY, &ParamData[index], true);
}

void SQLOperation::SetParamUInt16(uint8 index, const uint16& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_SHORT, &ParamData[index], true);
}

void SQLOperation::SetParamUInt32(uint8 index, const uint32& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_LONG, &ParamData[index], true);
}

void SQLOperation::SetParamUInt64(uint8 index, const uint64& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_LONGLONG, &ParamData[index], true);
}

void SQLOperation::SetParamInt8(uint8 index, const int8& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_TINY, &ParamData[index], false);
}

void SQLOperation::SetParamInt16(uint8 index, const int16& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_SHORT, &ParamData[index], false);
}

void SQLOperation::SetParamInt32(uint8 index, const int32& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_LONG, &ParamData[index], false);
}

void SQLOperation::SetParamInt64(uint8 index, const int64& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_LONGLONG, &ParamData[index], false);
}

void SQLOperation::SetParamFloat(uint8 index, const float& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_FLOAT, &ParamData[index], (value > 0.0f));
}

void SQLOperation::SetParamDouble(uint8 index, const double& value)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<const uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_DOUBLE, &ParamData[index], (value > 0.0f));
}

void SQLOperation::SetParamString(uint8 index, const char* value)
{
	ParamSetBitMask |= 1 << index;

	uint32 len = uint32(strlen(value)) + 1;
	char* stringLocation = new char[len];
	memcpy(stringLocation, value, len);

	ParamData[index] = reinterpret_cast<uint64&>(stringLocation);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_VAR_STRING, stringLocation, false, len, len - 1);
}

void SQLOperation::SetParamString(uint8 index, const std::string& value)
{
	ParamSetBitMask |= 1 << index;

	uint32 len = value.size() + 1;
	char* stringLocation = new char[len];
	memcpy(stringLocation, value.c_str(), len);

	ParamData[index] = reinterpret_cast<uint64&>(stringLocation);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_VAR_STRING, stringLocation, false, len, len - 1);
}

void SQLOperation::SetParamBinary(uint8 index, const void* value, uint32 dataSize)
{
	ParamSetBitMask |= 1 << index;

	char* binaryLocation = new char[dataSize];
	memcpy(binaryLocation, value, dataSize);

	ParamData[index] = reinterpret_cast<uint64&>(binaryLocation);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_BLOB, binaryLocation, false, dataSize, dataSize);
}

void SQLOperation::SetParamBinary(uint8 index, const std::vector<uint8>& value)
{
	ParamSetBitMask |= 1 << index;

	uint32 dataSize = value.size();
	char* binaryLocation = new char[dataSize];
	memcpy(binaryLocation, value.data(), dataSize);

	ParamData[index] = reinterpret_cast<uint64&>(binaryLocation);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_BLOB, binaryLocation, false, dataSize, dataSize);
}

void SQLOperation::MoveParamString(uint8 index, char*& value)
{
	ParamSetBitMask |= 1 << index;
	uint32 len = uint32(strlen(value)) + 1;
	ParamData[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_VAR_STRING, value, false, len, len - 1);
	value = nullptr;
}

void SQLOperation::MoveParamString(uint8 index, std::string&& value)
{
	ParamSetBitMask |= 1 << index;

	char temp[sizeof(std::string)] = { 0 };
	std::string* tempString = reinterpret_cast<std::string*>(&temp);
	*tempString = std::move(value);

	uint32 len = value.size() + 1;
	const char* stringLocation = value.c_str();

	ParamData[index] = reinterpret_cast<uint64&>(stringLocation);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_VAR_STRING, const_cast<void*>(reinterpret_cast<const void*>(stringLocation)), false, len, len - 1);
}

void SQLOperation::MoveParamBinary(uint8 index, void*& value, uint32 dataSize)
{
	ParamSetBitMask |= 1 << index;
	ParamData[index] = reinterpret_cast<uint64&>(value);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_BLOB, (void*)value, false, dataSize, dataSize);
	value = nullptr;
}

void SQLOperation::MoveParamBinary(uint8 index, std::vector<uint8>&& value)
{
	ParamSetBitMask |= 1 << index;

	uint32 dataSize = value.size();
	void* binaryLocation = value.data();

	ParamData[index] = reinterpret_cast<uint64&>(binaryLocation);
	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_BLOB, binaryLocation, false, dataSize, dataSize);
}

void SQLOperation::SetParamNull(uint8 index)
{
	ParamSetBitMask |= 1 << index;

	SetMySqlBind(&ParamBinds[index], MYSQL_TYPE_NULL, NULL, false, 0, 0, true);
}

bool SQLOperation::GetNextRowOfResultSet()
{
	if (FieldCount && CurrentRowCursor < RowCount)
	{
		CurrentRowCursor++;
		return true;
	}
	else
	{
		return false;
	}
}

bool SQLOperation::GetResultBool(uint8 index)
{
	return bool(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

uint8 SQLOperation::GetResultUInt8(uint8 index)
{
	return uint8(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

int8 SQLOperation::GetResultInt8(uint8 index)
{
	return int8(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

uint16 SQLOperation::GetResultUInt16(uint8 index)
{
	return uint16(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

int16 SQLOperation::GetResultInt16(uint8 index)
{
	return int16(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

uint32 SQLOperation::GetResultUInt32(uint8 index)
{
	return uint32(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

int32 SQLOperation::GetResultInt32(uint8 index)
{
	return int32(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

uint64 SQLOperation::GetResultUInt64(uint8 index)
{
	return uint64(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

int64 SQLOperation::GetResultInt64(uint8 index)
{
	return int64(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

float SQLOperation::GetResultFloat(uint8 index)
{
	return reinterpret_cast<float&>(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

double SQLOperation::GetResultDouble(uint8 index)
{
	return reinterpret_cast<double&>(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

char const* SQLOperation::GetResultCString(uint8 index)
{
	return reinterpret_cast<char const*>(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
}

std::string SQLOperation::GetResultString(uint8 index)
{
	return std::string(reinterpret_cast<char const*>(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]));
}

std::vector<uint8> SQLOperation::GetResultBinary(uint8 index, std::size_t size)
{
	std::vector<uint8> bytes(size, 0);
	void* binaryBuffer = reinterpret_cast<void*>(ResultSetData[(CurrentRowCursor - 1)*FieldCount + index]);
	int i = 0;
	for (auto& byte : bytes)
	{
		byte = ((uint8*)(binaryBuffer))[i++];
	}
	return bytes;
}

void SQLOperation::SetMySqlBind(MYSQL_BIND* mySqlBind, enum_field_types bufferType, void* bufferLocation, bool isUnsigned, uint32 bufferLength /*= 0*/, uint32 dataSize /*= 0*/, bool isNull /*= 0*/)
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

uint32 SQLOperation::SizeForType(MYSQL_FIELD* field)
{
	switch (field->type)
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
		return field->max_length + 1;

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
		//TODO error log
		return 0;
	}
}

bool SQLOperation::FetchNextRow()
{
	int retval = mysql_stmt_fetch(SqlStatement);
	return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
}
