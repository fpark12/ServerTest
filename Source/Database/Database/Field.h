/*
#pragma once
#include <mysql.h>

// Error Code
#define MYSQL_TYPE_NOT_RECOGNIZED_ERROR -1

class Field
{
	friend class ResultSet;
	friend class PreparedStatement;

protected:

	MYSQL_BIND* MysqlBind;



























#pragma pack(push, 1)
	struct data
	{
		uint32 length;          // Length (prepared strings only)
		void* value;            // Actual data in memory
		enum_field_types type;  // Field type (mysql types)
	};
#pragma pack(pop)

	struct Metadata
	{
		char const* TableName;
		char const* TableAlias;
		char const* Name;
		char const* Alias;
		char const* Type;
		uint32 Index;
	};

	static int32 SizeForType(MYSQL_FIELD* field);

	bool IsNumeric() const;

	bool IsBlobOrText() const;

	bool IsType(enum_field_types type) const;

	bool SetValue(uint32 _length, void* _value, enum_field_types _type);

	bool SetValue(void* _value, enum_field_types _type);

	void CleanUp();

public:
	Field();
	~Field();

	template <typename T>
	T* Get()
	{
		switch (data.type)
		{
		case MYSQL_TYPE_NULL:
			return nullptr;
			case MYSQL
		default:
			break;
		}
	}
	
	bool Bool() const
	{
		return GetUInt8() == 0 ? false : true;
	}

	uint8 GetUInt8() const
	{
		/ *if (!data.value)
			return 0;* /

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_TINY))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetUInt8() on non-tinyint field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<uint8*>(data.value);
		return static_cast<uint8>(strtoul((char*)data.value, nullptr, 10));
}

	int8 GetInt8() const
	{
		if (!data.value)
			return 0;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_TINY))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetInt8() on non-tinyint field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<int8*>(data.value);
		return static_cast<int8>(strtol((char*)data.value, NULL, 10));
	}

	uint16 GetUInt16() const
	{
		if (!data.value)
			return 0;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_SHORT) && !IsType(MYSQL_TYPE_YEAR))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetUInt16() on non-smallint field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<uint16*>(data.value);
		return static_cast<uint16>(strtoul((char*)data.value, nullptr, 10));
	}

	int16 GetInt16() const
	{
		if (!data.value)
			return 0;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_SHORT) && !IsType(MYSQL_TYPE_YEAR))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetInt16() on non-smallint field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<int16*>(data.value);
		return static_cast<int16>(strtol((char*)data.value, NULL, 10));
	}

	uint32 GetUInt32() const
	{
		if (!data.value)
			return 0;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_INT24) && !IsType(MYSQL_TYPE_LONG))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetUInt32() on non-(medium)int field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<uint32*>(data.value);
		return static_cast<uint32>(strtoul((char*)data.value, nullptr, 10));
	}

	int32 GetInt32() const
	{
		if (!data.value)
			return 0;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_INT24) && !IsType(MYSQL_TYPE_LONG))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetInt32() on non-(medium)int field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<int32*>(data.value);
		return static_cast<int32>(strtol((char*)data.value, NULL, 10));
	}

	uint64 GetUInt64() const
	{
		if (!data.value)
			return 0;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_LONGLONG) && !IsType(MYSQL_TYPE_BIT))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetUInt64() on non-bigint field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<uint64*>(data.value);
		return static_cast<uint64>(strtoull((char*)data.value, nullptr, 10));
	}

	int64 GetInt64() const
	{
		if (!data.value)
			return 0;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_LONGLONG) && !IsType(MYSQL_TYPE_BIT))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetInt64() on non-bigint field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<int64*>(data.value);
		return static_cast<int64>(strtoll((char*)data.value, NULL, 10));
	}

	float GetFloat() const
	{
		if (!data.value)
			return 0.0f;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_FLOAT))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetFloat() on non-float field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0.0f;
		}
#endif

		if (data.raw)
			return *reinterpret_cast<float*>(data.value);
		return static_cast<float>(atof((char*)data.value));
	}

	double GetDouble() const
	{
		if (!data.value)
			return 0.0f;

#ifdef TRINITY_DEBUG
		if (!IsType(MYSQL_TYPE_DOUBLE) && !IsType(MYSQL_TYPE_NEWDECIMAL))
		{
			TC_LOG_WARN("sql.sql", "Warning: GetDouble() on non-double/non-decimal field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return 0.0f;
		}
#endif

		if (data.raw && !IsType(MYSQL_TYPE_NEWDECIMAL))
			return *reinterpret_cast<double*>(data.value);
		return static_cast<double>(atof((char*)data.value));
	}

	char const* GetCString() const
	{
		if (!data.value)
			return NULL;

#ifdef TRINITY_DEBUG
		if (IsNumeric())
		{
			TC_LOG_WARN("sql.sql", "Error: GetCString() on numeric field %s.%s (%s.%s) at index %u. Using type: %s.",
				meta.TableAlias, meta.Alias, meta.TableName, meta.Name, meta.Index, meta.Type);
			return NULL;
		}
#endif
		return static_cast<char const*>(data.value);
	}

	std::string GetString() const
	{
		if (!data.value)
			return "";

		char const* string = GetCString();
		if (!string)
			return "";

		return std::string(string, data.length);
	}

	std::vector<uint8> GetBinary() const
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

};*/