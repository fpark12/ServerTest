#include "Bind.h"

Bind::Bind(std::vector<QueryParamSizeInfo>&& paramSizeInfoList) :
	ParamCount(paramSizeInfoList.size()), ParamBuffer(nullptr), BufferIterators(),
	totalSize(0)
{
	ASSERT(ParamCount <= MaxParamCount);
	sumOfSize(paramSizeInfoList);
	ParamBuffer = new uint8[totalSize];
	int bufferOffset = 0;
	for (std::size_t i = 0;
		i < paramSizeInfoList.size() && bufferOffset < totalSize;
		++i)
	{
		BufferIterators[i]->ParamPtr = (void*)(ParamBuffer + bufferOffset);
		BufferIterators[i]->ParamType = paramSizeInfoList[i].Type;
		if (paramSizeInfoList[i].Type == TYPE_STRING ||
			paramSizeInfoList[i].Type == TYPE_BINARY)
		{
			BufferIterators[i]->Max_Length = paramSizeInfoList[i].MaxDataLength;
			bufferOffset += paramSizeInfoList[i].MaxDataLength;
		}
		else
		{
			bufferOffset += ParamSize(paramSizeInfoList[i].Type);
		}
	}
}

int32 Bind::ParamSize(QueryParamType Type)
{
	switch (Type)
	{
	case TYPE_BOOL:
		return sizeof(bool);
	case TYPE_UI8:
		return sizeof(uint8);
	case TYPE_UI16:
		return sizeof(uint16);
	case TYPE_UI32:
		return sizeof(uint32);
	case TYPE_UI64:
		return sizeof(uint64);
	case TYPE_I8:
		return sizeof(int8);
	case TYPE_I16:
		return sizeof(int16);
	case TYPE_I32:
		return sizeof(int32);
	case TYPE_I64:
		return sizeof(int64);
	case TYPE_FLOAT:
		return sizeof(float);
	case TYPE_DOUBLE:
		return sizeof(double);
	case TYPE_STRING:
	case TYPE_BINARY:
		return -1;
	case TYPE_NULL:
		return 0;
	default:
		//TODO error log system
		throw std::invalid_argument("Received invalid param type");
	}
}

std::size_t Bind::sumOfSize(std::vector<QueryParamSizeInfo>& paramSizeInfoList)
{
	for (QueryParamSizeInfo& paramSizeInfo : paramSizeInfoList)
	{
		if (paramSizeInfo.Type == TYPE_STRING || paramSizeInfo.Type == TYPE_BINARY)
		{
			totalSize += paramSizeInfo.MaxDataLength;
		}
		else
		{
			totalSize += ParamSize(paramSizeInfo.Type);
		}
	}
	return totalSize;
}
