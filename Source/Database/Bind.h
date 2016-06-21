#pragma once
#include <vector>
#include <cstdio>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include "Define.h"
#include "Errors.h"


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
public:
	Bind(std::vector<QueryParamSizeInfo>&& paramSizeInfoList);
	
	template<typename T>
	void SetParam(uint8 index, T&& value)
	{
		auto iter = BufferIterators[index];
		if (iter->ParamType == TYPE_STRING || iter->ParamType == TYPE_BINARY)
		{
			memcpy(iter->ParamPtr, &(value), std::min(sizeof(value), iter->Max_Length));
		} 
		else
		{
			memcpy(iter->ParamPtr, &(value), ParamSize(iter->ParamType));
		}
	}

	template<typename T>
	const T* const GetParam(uint8 index)
	{
		return (T*)(BufferIterators[index]->ParamPtr);
	}
private:

	std::size_t sumOfSize(std::vector<QueryParamSizeInfo>& paramSizeInfoList);

	int32 ParamSize(QueryParamType Type);

	struct Header
	{
		void* ParamPtr;
		QueryParamType ParamType;
		unsigned long Max_Length;
	};

	static const int MaxParamCount = QueryMaxParamCount;
	int ParamCount;
	uint8* ParamBuffer;
	std::vector<std::unique_ptr<Header>> BufferIterators;
	std::size_t totalSize;

	Bind(Bind const& right) = delete;
	Bind& operator=(Bind const& right) = delete;
};