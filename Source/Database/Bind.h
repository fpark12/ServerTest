#pragma once
#include <vector>
#include <cstdio>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <type_traits>
#include "Define.h"
#include "Errors.h"

class PreparedStatement;

enum QueryParamType
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
	QueryParamType Type;
	std::size_t MaxDataLength;
};

class Bind
{
private:

	friend class PreparedStatement;

public:

	Bind(std::vector<QueryParamSizeInfo>&& paramSizeInfoList);
	~Bind();

protected:

	template<typename T>
	void SetParam(const uint8 index, const T&& value)
	{
		auto iter = BufferIterators[index];
		ASSERT(iter.ParamType != TYPE_STRING && iter.ParamType != TYPE_BINARY && iter.ParamType != TYPE_NULL);
		memcpy(iter.ParamPtr, (const void*)&(value), ParamSize(iter.ParamType));
		iter.IsBinded = true;
	}

	template<>
	void SetParam<const char*>(const uint8 index, const char* const&& value)
	{
		auto iter = BufferIterators[index];
		ASSERT(iter.ParamType == TYPE_STRING || iter.ParamType == TYPE_BINARY);
		memcpy(iter.ParamPtr, (const void*)value, ::std::min(sizeof(value), iter.MaxLength));
		iter.IsBinded = true;
	}

	template<>
	void SetParam<void*>(const uint8 index, void* const&& value)
	{
		auto iter = BufferIterators[index];
		ASSERT(iter.ParamType == TYPE_NULL || !value);
		iter.IsBinded = true;
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

	QueryParamType GetParamType(uint8 index);

	/* Get Max Length of a string parameter. If the parameter has a type other string or binary,
	*  the function will return 0.
	*/
	unsigned long GetStringParamMaxLength(uint8 index);

private:

	std::size_t sumOfSize(std::vector<QueryParamSizeInfo>& paramSizeInfoList);

	int32 ParamSize(QueryParamType Type);

	struct Header
	{
		void* ParamPtr = nullptr;
		QueryParamType ParamType = TYPE_NULL;
		size_t MaxLength = 0;
		bool IsBinded = false;
	};

	static const int MaxParamCount = QueryMaxParamCount;
	int ParamCount;
	uint8* ParamBuffer;
	std::vector<Header> BufferIterators;
	std::size_t totalSize;

	DISALLOW_COPY(Bind);
};