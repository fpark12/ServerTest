/*
#pragma once

#include "Core.h"

class PreparedStatement;

enum QueryParamCPPType
{
	TYPE_BOOL,
	TYPE_UI8,
	TYPE_UI16,
	TYPE_UI32,
	TYPE_UI64,
	TYPE_I8,
	TYPE_I16,
	TYPE_I32,
	TYPE_I64,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_BINARY,
	TYPE_NULL
};

struct QueryParamSizeInfo
{
	QueryParamCPPType Type;
	std::size_t MaxDataLength;
};

class ParamBind
{
private:

	friend class PreparedStatement;

public:

	ParamBind(std::vector<QueryParamSizeInfo>&& paramSizeInfoList);
	~ParamBind();

protected:

	template<typename T>
	void SetParam(const uint8 index, const T&& value)
	{
		ASSERT(ParamsCppType[index] != TYPE_STRING && ParamsCppType[index] != TYPE_BINARY && ParamsCppType[index] != TYPE_NULL);
		memcpy(MySqlBinds[index].buffer, (const void*)&(value), ParamSize(ParamsCppType[index]));
		bValueSet[index] = true;
	}

	template<>
	void SetParam<const char*>(const uint8 index, const char* const&& value)
	{
		ASSERT(ParamsCppType[index] == TYPE_STRING || ParamsCppType[index] == TYPE_BINARY);
		memcpy(MySqlBinds[index].buffer, value, strlen(value));
		delete MySqlBinds[index].length;
		MySqlBinds[index].length = new unsigned long(strlen(value));
		bValueSet[index] = true;
	}

	template<>
	void SetParam<void*>(const uint8 index, void* const&& value)
	{
		ASSERT(ParamsCppType[index] == TYPE_NULL || !value);
		bValueSet[index] = true;
	}

	template<typename T>
	const T* const GetParam(const uint8 index)
	{
		if (BufferIterators[index].IsBinded)
		{
			return (T*)(BufferIterators[index].ParamPtr);
		}
		else
		{
			return nullptr;
		}
	}

	void ClearParameters();

	QueryParamCPPType GetParamType(uint8 index);

	/ * Get Max Length of a string parameter. If the parameter has a type other string or binary,
	*  the function will return 0.
	* /
	unsigned long GetStringParamMaxLength(uint8 index);

private:

	std::size_t sumOfSize(std::vector<QueryParamSizeInfo>& paramSizeInfoList);

	int32 ParamSize(QueryParamCPPType Type);

	void SetMySqlBindTypeInfo(QueryParamSizeInfo& paramSizeInfo, MYSQL_BIND& bind);

	static const int MaxParamCount = QueryMaxParamCount;
	int ParamCount;
	uint8* ParamBuffer;
	MYSQL_BIND* MySqlBinds;
	std::vector<QueryParamCPPType> ParamsCppType;
	std::vector<bool> bValueSet;
	std::size_t TotalParamSize;

	DISALLOW_COPY(ParamBind);
};*/