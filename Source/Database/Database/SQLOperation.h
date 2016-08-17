#pragma once
#include <mysql.h>

class DatabaseConnection;

enum class StatementStorage
{
	Login,
	World,
	Character,
	Hotfix,
	None
};

enum class SqlOperationFlag
{
	Sync,
	Async,
	Both,
	Neither
};

enum class SQLOperationResult
{
	Success,
	Failed,
	None,
	Count
};

class SQLOperation
{

private:

	// General
	DatabaseConnection* SqlConnection;
	MYSQL_STMT* SqlStatement;
	SqlOperationFlag OperationFlag;

	// Params: for input
	uint32 ParamCount;
	uint32 ParamSetBitMask;	// A max of 32 params are allowed for each query
	MYSQL_BIND* ParamBinds;
	uint64* ParamData;
	StatementStorage Storage; // todo: ??

	// Field: for output
	MYSQL_BIND* FieldBinds;
	uint64 RowCount;
	uint32 FieldCount;
	uint64 CurrentRowCursor;
	uint64* RowData;
	uint64* ResultSetData;

	// Status
	std::atomic<SQLOperationResult> SQLOperationResult;


	DISALLOW_COPY(SQLOperation);

public:

	SQLOperation(DatabaseConnection* conn);
	~SQLOperation();

	void ClearParam();
	void ClearResult();

	void Clear()
	{
		ClearParam();
		ClearResult();
		SqlStatement = nullptr;
		OperationFlag = SqlOperationFlag::Neither;
		ParamCount = 0;
		ParamSetBitMask = 0x00000000;
		ParamData = nullptr;
		Storage = StatementStorage::None;
		RowCount = 0;
		FieldCount = 0;
		CurrentRowCursor = 0;
		SQLOperationResult = SQLOperationResult::None;
	}

	// init params
	void SetConnection(DatabaseConnection* conn);
	void SetStatement(MYSQL_STMT* stmt);
	void SetStatement(char* sql);
	void SetOperationFlag(SqlOperationFlag flag);
	void Execute();
	void Call();
	bool IsDone()
	{
		if (SQLOperationResult == SQLOperationResult::Success)
		{
			return true;
		}

		return false;
	}

	void SetParamBool(uint8 index, const bool& value);
	void SetParamUInt8(uint8 index, const uint8& value);
	void SetParamUInt16(uint8 index, const uint16& value);
	void SetParamUInt32(uint8 index, const uint32& value);
	void SetParamUInt64(uint8 index, const uint64& value);
	void SetParamInt8(uint8 index, const int8& value);
	void SetParamInt16(uint8 index, const int16& value);
	void SetParamInt32(uint8 index, const int32& value);
	void SetParamInt64(uint8 index, const int64& value);
	void SetParamFloat(uint8 index, const float& value);
	void SetParamDouble(uint8 index, const double& value);
	void SetParamString(uint8 index, const char* value);
	void SetParamString(uint8 index, const std::string& value);
	void SetParamBinary(uint8 index, const void* value, uint32 dataSize);
	void SetParamBinary(uint8 index, const std::vector<uint8>& value);
	void SetParamNull(uint8 index);

	void MoveParamString(uint8 index, char*& value);
	void MoveParamString(uint8 index, std::string&& value);
	void MoveParamBinary(uint8 index, void*& value, uint32 dataSize);
	void MoveParamBinary(uint8 index, std::vector<uint8>&& value);

	bool GetNextRowOfResultSet();

	bool GetResultBool(uint8 index);
	uint8 GetResultUInt8(uint8 index);
	uint16 GetResultUInt16(uint8 index);
	uint32 GetResultUInt32(uint8 index);
	uint64 GetResultUInt64(uint8 index);
	int8 GetResultInt8(uint8 index);
	int16 GetResultInt16(uint8 index);
	int32 GetResultInt32(uint8 index);
	int64 GetResultInt64(uint8 index);
	
	float GetResultFloat(uint8 index);
	double GetResultDouble(uint8 index);
	char const* GetResultCString(uint8 index);
	std::string GetResultString(uint8 index);
	std::vector<uint8> GetResultBinary(uint8 index, std::size_t size);

private:

	// Utility Function for set params
	void SetMySqlBind(MYSQL_BIND* mySqlBind, enum_field_types bufferType, void* bufferLocation,
		bool isUnsigned, uint32 bufferLength = 0, uint32 dataSize = 0, bool isNull = 0);

	static uint32 SizeForType(MYSQL_FIELD* field);

	bool FetchNextRow();

};