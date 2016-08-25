#pragma once

struct SQLOperationResultSet
{
	enum class SQLOperationStatus
	{
		Success,
		Failed,
		None,
		Count
	};

	// Field: for output
	MYSQL_BIND* FieldBinds;
	uint64 RowCount;
	uint32 FieldCount;
	uint64 CurrentRowCursor;
	uint64* RowData;
	uint64* ResultSetData;

	// Status
	std::atomic<SQLOperationStatus> SQLOperationStatus = SQLOperationStatus::None;

	void ClearResultSet();
	bool GetNextRowOfResultSet();

	bool GetResultBool(uint8 Index);
	uint8 GetResultUInt8(uint8 Index);
	uint16 GetResultUInt16(uint8 Index);
	uint32 GetResultUInt32(uint8 Index);
	uint64 GetResultUInt64(uint8 Index);
	int8 GetResultInt8(uint8 Index);
	int16 GetResultInt16(uint8 Index);
	int32 GetResultInt32(uint8 Index);
	int64 GetResultInt64(uint8 Index);

	float GetResultFloat(uint8 Index);
	double GetResultDouble(uint8 Index);
	char const* GetResultCString(uint8 Index);
	std::string GetResultString(uint8 Index);
	std::vector<uint8> GetResultBinary(uint8 Index, std::size_t Size);


};