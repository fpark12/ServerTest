#include "Private//Detail/SQLOpertion/SQLOperationResultSet.h"

void SQLOperationResultSet::ClearResultSet()
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

bool SQLOperationResultSet::GetNextRowOfResultSet()
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

bool SQLOperationResultSet::GetResultBool(uint8 Index)
{
	return !!(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

uint8 SQLOperationResultSet::GetResultUInt8(uint8 Index)
{
	return uint8(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

int8 SQLOperationResultSet::GetResultInt8(uint8 Index)
{
	return int8(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

uint16 SQLOperationResultSet::GetResultUInt16(uint8 Index)
{
	return uint16(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

int16 SQLOperationResultSet::GetResultInt16(uint8 Index)
{
	return int16(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

uint32 SQLOperationResultSet::GetResultUInt32(uint8 Index)
{
	return uint32(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

int32 SQLOperationResultSet::GetResultInt32(uint8 Index)
{
	return int32(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

uint64 SQLOperationResultSet::GetResultUInt64(uint8 Index)
{
	return uint64(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

int64 SQLOperationResultSet::GetResultInt64(uint8 Index)
{
	return int64(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

float SQLOperationResultSet::GetResultFloat(uint8 Index)
{
	return reinterpret_cast<float&>(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

double SQLOperationResultSet::GetResultDouble(uint8 Index)
{
	return reinterpret_cast<double&>(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

char const* SQLOperationResultSet::GetResultCString(uint8 Index)
{
	return reinterpret_cast<char const*>(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
}

std::string SQLOperationResultSet::GetResultString(uint8 Index)
{
	return std::string(reinterpret_cast<char const*>(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]));
}

std::vector<uint8> SQLOperationResultSet::GetResultBinary(uint8 Index, std::size_t size)
{
	std::vector<uint8> bytes(size, 0);
	void* binaryBuffer = reinterpret_cast<void*>(ResultSetData[(CurrentRowCursor - 1) * FieldCount + Index]);
	int i = 0;
	for (auto& byte : bytes)
	{
		byte = ((uint8*)(binaryBuffer))[i++];
	}
	return bytes;
}