#pragma once

struct SQLOperationResultSet
{

	// Field: for output
	MYSQL_BIND* FieldBinds = nullptr;
	uint64 RowCount = 0;
	uint32 FieldCount = 0;
	uint64 CurrentRowCursor = 0;
	uint64* RowData = nullptr;
	uint64* ResultSetData = nullptr;

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

	uint32 SizeForType(MYSQL_FIELD* field);

};