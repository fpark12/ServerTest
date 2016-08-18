#pragma once

struct SQLOperationParamsArchive
{
	enum class StatementStorage
	{
		Login,
		World,
		Character,
		Hotfix,
		None
	};

	// Params: for input
	uint32 ParamCount;
	uint32 ParamSetBitMask;	// A max of 32 params are allowed for each query
	MYSQL_BIND* ParamBinds;
	uint64* ParamData;
	StatementStorage Storage; // todo: ?? what is this

	void SetParamBind(MYSQL_BIND* mySqlBind, enum_field_types bufferType, void* bufferLocation,
		bool isUnsigned, size_t bufferLength = 0, size_t dataSize = 0, bool isNull = 0);

	void ClearParams();
	void SetParamBool(uint8 Index, const bool& Value);
	void SetParamUInt8(uint8 Index, const uint8& Value);
	void SetParamUInt16(uint8 Index, const uint16& Value);
	void SetParamUInt32(uint8 Index, const uint32& Value);
	void SetParamUInt64(uint8 Index, const uint64& Value);
	void SetParamInt8(uint8 Index, const int8& Value);
	void SetParamInt16(uint8 Index, const int16& Value);
	void SetParamInt32(uint8 Index, const int32& Value);
	void SetParamInt64(uint8 Index, const int64& Value);
	void SetParamFloat(uint8 Index, const float& Value);
	void SetParamDouble(uint8 Index, const double& Value);
	void SetParamString(uint8 Index, const char* Value);
	void SetParamString(uint8 Index, const std::string& Value);
	void SetParamBinary(uint8 Index, const void* Value, uint32 dataSize);
	void SetParamBinary(uint8 Index, const std::vector<uint8>& Value);
	void SetParamNull(uint8 Index);

	void MoveParamString(uint8 Index, char*& Value);
	void MoveParamString(uint8 Index, std::string&& Value);
	void MoveParamBinary(uint8 Index, void*& Value, uint32 dataSize);
	void MoveParamBinary(uint8 Index, std::vector<uint8>&& Value);
};