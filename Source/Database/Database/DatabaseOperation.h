#pragma once
#include <mysql.h>

enum StatementStorage
{
	Login,
	World,
	Character,
	Hotfix,
	None
};

enum SqlOperationFlag
{
	Sync,
	Async,
	Both,
	Neither
};

class DatabaseOperation
{

private:

	// General
	MYSQL_STMT* MySqlStatementHandle;
	SqlOperationFlag OperationFlag;

	// Params: for input
	uint32 ParamCount;
	uint32 ParamSetMask;	// A max of 32 param is allowed for each query
	MYSQL_BIND* ParamBindHandle;
	uint64* ParamDataSerialization;
	StatementStorage Storage;

	// Field: for output
	bool HasResult;
	MYSQL_RES* ResultMetaData;
	MYSQL_FIELD* ResultDataFields;
	MYSQL_BIND* FieldBindHandle;
	uint64 RowCount;
	uint32 FieldCount;
	uint64 CurrentRowCursor;
	uint64* RowDataSerialization;
	uint64* ResultSetDataSerialization;

	// Database Connection


	DISALLOW_COPY(DatabaseOperation);

public:

	DatabaseOperation();
	~DatabaseOperation();

	void ClearParam();
	void ClearResult();

	// init params
	// TODO void SetConnection(DatabaseConnection* conn);
	void SetStatement(MYSQL_STMT* stmt);
	void SetOperationFlag(SqlOperationFlag flag);
	void ExecuteStatement();
	
	void SetParamBool(uint8 index, bool&& value);
	void SetParamUInt8(uint8 index, uint8&& value);
	void SetParamInt8(uint8 index, int8&& value);
	void SetParamUInt16(uint8 index, uint16&& value);
	void SetParamInt16(uint8 index, int16&& value);
	void SetParamUInt32(uint8 index, uint32&& value);
	void SetParamInt32(uint8 index, int32&& value);
	void SetParamUInt64(uint8 index, uint64&& value);
	void SetParamInt64(uint8 index, int64&& value);
	void SetParamFloat(uint8 index, float&& value);
	void SetParamDouble(uint8 index, double&& value);
	void SetParamString(uint8 index, char const* value);
	void SetParamString(uint8 index, std::string&& value);
	void SetParamBinary(uint8 index, const void* value, uint32 dataSize);
	void SetParamBinary(uint8 index, std::vector<uint8>&& value);
	void SetParamNull(uint8 index);

	bool GetNextRowOfResultSet();
	
	bool GetBool(uint8 index);
	uint8 GetUInt8(uint8 index);
	int8 GetInt8(uint8 index);
	uint16 GetUInt16(uint8 index);
	int16 GetInt16(uint8 index);
	uint32 GetUInt32(uint8 index);
	int32 GetInt32(uint8 index);
	uint64 GetUInt64(uint8 index);
	int64 GetInt64(uint8 index);
	float GetFloat(uint8 index);
	double GetDouble(uint8 index);
	char const* GetCString(uint8 index);
	std::string GetString(uint8 index);
	std::vector<uint8> GetBinary(uint8 index, std::size_t size);

private:

	// Utility Function for set params
	void SetMySqlBind(MYSQL_BIND* mySqlBind, enum_field_types bufferType, void* bufferLocation,
		bool isUnsigned, uint32 bufferLength = 0, uint32 dataSize = 0, bool isNull = 0);

	static uint32 SizeForType(MYSQL_FIELD* field);

	bool FetchNextRow();

};