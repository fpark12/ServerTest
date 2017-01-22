#pragma once
#include <openssl/sha.h>

class BigNumber;

class Core_API SHA256Hash
{
	public:
		typedef std::integral_constant<uint32, SHA256_DIGEST_LENGTH> DigestLength;

		SHA256Hash();
		~SHA256Hash();

		void UpdateBigNumbers(BigNumber* bn0, ...);

		void AppendData(const uint8 *Data, int Len);
		void AppendData(const std::string& String);

		SHA256Hash& operator << (const std::string& String);

		void Initialize();
		void CalculateHash();

		uint8* GetDigest(void) { return Digest; }
		int GetLength(void) const { return SHA256_DIGEST_LENGTH; }

		std::string GetHexString();
	private:
		SHA256_CTX SHA256Context;
		uint8 Digest[SHA256_DIGEST_LENGTH];
};
