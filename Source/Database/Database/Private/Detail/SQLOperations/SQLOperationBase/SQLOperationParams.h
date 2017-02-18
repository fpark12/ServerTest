#pragma once

struct SQLOperationParams
{

	/*
	enum class StatementStorage
	{
		Login,
		World,
		Character,
		Hotfix,
		None
	};
	*/

	// Params: for input
	uint32 ParamCount = 0;
	uint32 ParamSetBitMask = 0x00000000;	// A max of 32 params are allowed for each query
	MYSQL_BIND* ParamBinds = nullptr;
	uint64* ParamData = nullptr;
	// StatementStorage Storage; // todo: ?? what is this
	
	void ClearParams();

	void SetBool(uint8 Index, const bool& Value);
	void SetUInt8(uint8 Index, const uint8& Value);
	void SetUInt16(uint8 Index, const uint16& Value);
	void SetUInt32(uint8 Index, const uint32& Value);
	void SetUInt64(uint8 Index, const uint64& Value);
	void SetInt8(uint8 Index, const int8& Value);
	void SetInt16(uint8 Index, const int16& Value);
	void SetInt32(uint8 Index, const int32& Value);
	void SetInt64(uint8 Index, const int64& Value);
	void SetFloat(uint8 Index, const float& Value);
	void SetDouble(uint8 Index, const double& Value);
	void SetString(uint8 Index, const char* Value);
	void SetString(uint8 Index, const std::string& Value);
	void SetBinary(uint8 Index, const void* Value, uint32 dataSize);
	void SetBinary(uint8 Index, const std::vector<uint8>& Value);
	void SetNull(uint8 Index);

	void MoveString(uint8 Index, char*& Value);
	void MoveString(uint8 Index, std::string&& Value);
	void MoveBinary(uint8 Index, void*& Value, uint32 dataSize);
	void MoveBinary(uint8 Index, std::vector<uint8>&& Value);

	void SetBind(MYSQL_BIND* mySqlBind, enum_field_types bufferType, void* bufferLocation,
		bool isUnsigned, size_t bufferLength = 0, size_t dataSize = 0, bool isNull = 0);
};