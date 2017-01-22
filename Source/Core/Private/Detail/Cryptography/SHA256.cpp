#include "SHA256.h"
#include "BigNumber.h"
#include <cstring>
#include <stdarg.h>

SHA256Hash::SHA256Hash()
{
	SHA256_Init(&SHA256Context);
	memset(Digest, 0, SHA256_DIGEST_LENGTH * sizeof(uint8));
}

SHA256Hash::~SHA256Hash()
{
	// Seems unnecessary
	//SHA256_Init(&SHA256Context);
}

void SHA256Hash::AppendData(const uint8 *Data, int length)
{
	SHA256_Update(&SHA256Context, Data, length);
}

void SHA256Hash::AppendData(const std::string& String)
{
	AppendData((const uint8*)String.c_str(), String.length());
}

SHA256Hash & SHA256Hash::operator<<(const std::string & String)
{
	AppendData((const uint8*)String.c_str(), String.length());
	return *this;
}

void SHA256Hash::UpdateBigNumbers(BigNumber* bn0, ...)
{
	va_list v;
	BigNumber* bn;

	va_start(v, bn0);
	bn = bn0;
	while (bn)
	{
		AppendData(bn->AsByteArray().get(), bn->GetNumBytes());
		bn = va_arg(v, BigNumber*);
	}
	va_end(v);
}

void SHA256Hash::Initialize()
{
	SHA256_Init(&SHA256Context);
}

void SHA256Hash::CalculateHash(void)
{
	SHA256_Final(Digest, &SHA256Context);
}

constexpr char HexMap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

std::string SHA256Hash::GetHexString()
{
	int Len = this->GetLength();
	unsigned char* Data = this->GetDigest();

	std::string Result(Len * 2, ' ');
	for (int i = 0; i < Len; ++i) {
		Result[2 * i] = HexMap[(Data[i] & 0xF0) >> 4];
		Result[2 * i + 1] = HexMap[Data[i] & 0x0F];
	}
	return Result;
}
