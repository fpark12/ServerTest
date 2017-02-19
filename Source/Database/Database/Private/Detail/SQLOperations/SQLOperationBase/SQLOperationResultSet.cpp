#include "Private/Detail/SQLOperations/SQLOperationBase/SQLOperationResultSet.h"

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

size_t SQLOperationResultSet::SizeOfField(MYSQL_FIELD* field)
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
