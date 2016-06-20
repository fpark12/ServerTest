/*
* Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "Common.h"
#include "Define.h"
#include <vector>
#include <mysql.h>

class Field
{
	friend class ResultSet;
	friend class PreparedResultSet;

public:
	Field();
	~Field();

	bool Bool() const // Wrapper, actually gets integer
	{
		return UInt8() == 1 ? true : false;
	}
	
	uint8 UInt8() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<uint8*>(data.value);
		return static_cast<uint8>(strtoul((char*)data.value, nullptr, 10));
	}

	int8 Int8() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<int8*>(data.value);
		return static_cast<int8>(strtol((char*)data.value, NULL, 10));
	}

	uint16 UInt16() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<uint16*>(data.value);
		return static_cast<uint16>(strtoul((char*)data.value, nullptr, 10));
	}

	int16 Int16() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<int16*>(data.value);
		return static_cast<int16>(strtol((char*)data.value, NULL, 10));
	}

	uint32 UInt32() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<uint32*>(data.value);
		return static_cast<uint32>(strtoul((char*)data.value, nullptr, 10));
	}

	int32 Int32() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<int32*>(data.value);
		return static_cast<int32>(strtol((char*)data.value, NULL, 10));
	}

	uint64 UInt64() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<uint64*>(data.value);
		return static_cast<uint64>(strtoull((char*)data.value, nullptr, 10));
	}

	int64 Int64() const
	{
		if (!data.value)
			return 0;

		if (data.raw)
			return *reinterpret_cast<int64*>(data.value);
		return static_cast<int64>(strtoll((char*)data.value, NULL, 10));
	}

	float Float() const
	{
		if (!data.value)
			return 0.0f;

		if (data.raw)
			return *reinterpret_cast<float*>(data.value);
		return static_cast<float>(atof((char*)data.value));
	}

	double Double() const
	{
		if (!data.value)
			return 0.0f;

		if (data.raw && !IsType(MYSQL_TYPE_NEWDECIMAL))
			return *reinterpret_cast<double*>(data.value);
		return static_cast<double>(atof((char*)data.value));
	}

	char const* CString() const
	{
		if (!data.value)
			return NULL;

		return static_cast<char const*>(data.value);
	}

	std::string String() const
	{
		if (!data.value)
			return "";

		char const* string = CString();
		if (!string)
			return "";

		return std::string(string, data.length);
	}

	std::vector<uint8> Binary() const
	{
		std::vector<uint8> result;
		if (!data.value || !data.length)
			return result;

		result.resize(data.length);
		memcpy(result.data(), data.value, data.length);
		return result;
	}

	bool IsNull() const
	{
		return data.value == NULL;
	}

	struct Metadata
	{
		char const* TableName;
		char const* TableAlias;
		char const* Name;
		char const* Alias;
		char const* Type;
		uint32 Index;
	};

protected:
#pragma pack(push, 1)
	struct
	{
		uint32 length;          // Length (prepared strings only)
		void* value;            // Actual data in memory
		enum_field_types type;  // Field type
		bool raw;               // Raw bytes? (Prepared statement or ad hoc)
	} data;
#pragma pack(pop)

	void SetByteValue(void* newValue, enum_field_types newType, uint32 length);
	void SetStructuredValue(char* newValue, enum_field_types newType, uint32 length);

	void CleanUp()
	{
		// Field does not own the data if fetched with prepared statement
		if (!data.raw)
			delete[]((char*)data.value);
		data.value = NULL;
	}

	static uint32 SizeForType(MYSQL_FIELD* field)
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
			//TODO error message
			throw new std::runtime_error("SQL::SizeForType(): invalid field type");
			return 0;
		}
	}

	bool IsType(enum_field_types type) const
	{
		return data.type == type;
	}

	bool IsNumeric() const
	{
		return (data.type == MYSQL_TYPE_TINY ||
			data.type == MYSQL_TYPE_SHORT ||
			data.type == MYSQL_TYPE_INT24 ||
			data.type == MYSQL_TYPE_LONG ||
			data.type == MYSQL_TYPE_FLOAT ||
			data.type == MYSQL_TYPE_DOUBLE ||
			data.type == MYSQL_TYPE_LONGLONG);
	}

};

