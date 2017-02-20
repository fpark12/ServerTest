#pragma once
#include "utf8.h"

namespace String
{
	template <typename... Args>
	std::string Format(const char* format, const Args & ... args)
	{
		return fmt::format(format, args...);
	}

}

// http://stackoverflow.com/questions/28675727/using-crc32-algorithm-to-hash-string-at-compile-time
namespace cexp
{

	// Small implementation of std::array, needed until constexpr
	// is added to the function 'reference operator[](size_type)'
	template <typename T, std::size_t N>
	struct array {
		T m_data[N];

		using value_type = T;
		using reference = value_type &;
		using const_reference = const value_type &;
		using size_type = std::size_t;

		// This is NOT constexpr in std::array until C++17
		constexpr reference operator[](size_type i) noexcept
		{
			return m_data[i];
		}

		constexpr const_reference operator[](size_type i) const noexcept
		{
			return m_data[i];
		}

		constexpr size_type size() const noexcept
		{
			return N;
		}
	};

}

// Generates CRC-32 table, algorithm based from this link:
// http://www.hackersdelight.org/hdcodetxt/crc.c.txt
constexpr auto gen_crc32_table() {
	constexpr auto num_bytes = 256;
	constexpr auto num_iterations = 8;
	constexpr auto polynomial = 0xEDB88320;

	auto crc32_table = cexp::array<uint32_t, num_bytes>{};

	for (auto byte = 0u; byte < num_bytes; ++byte)
	{
		auto crc = byte;

		for (auto i = 0; i < num_iterations; ++i)
		{
			auto mask = -(crc & 1);
			crc = (crc >> 1) ^ (polynomial & mask);
		}

		crc32_table[byte] = crc;
	}

	return crc32_table;
}

// Stores CRC-32 table and softly validates it.
static constexpr auto crc32_table = gen_crc32_table();
static_assert(
	crc32_table.size() == 256 &&
	crc32_table[1] == 0x77073096 &&
	crc32_table[255] == 0x2D02EF8D,
	"gen_crc32_table generated unexpected result."
	);

// Generates CRC-32 code from null-terminated, c-string,
// algorithm based from this link:
// http://www.hackersdelight.org/hdcodetxt/crc.c.txt 
constexpr auto crc32(const char *in)
{
	auto crc = 0xFFFFFFFFu;

	for (auto i = 0u; auto c = in[i]; ++i) {
		crc = crc32_table[(crc ^ c) & 0xFF] ^ (crc >> 8);
	}

	return ~crc;
}

/*
int main()
{
	constexpr auto crc_code = crc32("some-id");
	static_assert(crc_code == 0x1A61BC96, "crc32 generated unexpected result.");

	std::cout << std::hex << crc_code << std::endl;
}
*/
/*
class String : public std::string
{
public:
	String()
	{
	}

	template <typename... Args>
	String(const char* format, const Args & ... args) :
	std::string(fmt::format(format, args...))
	{
	}

	/*
	FMT_VARIADIC(std::string, format, CStringRef)
	FMT_VARIADIC_(char, std::string, format, return format, __VA_ARGS__)

		# define FMT_VARIADIC_(Char, ReturnType, func, call, ...) \
	template <typename... Args> \
		std::string format(FMT_FOR_EACH(FMT_ADD_ARG_NAME, __VA_ARGS__), \
			const Args & ... args) {
		\
			typedef fmt::internal::ArgArray<sizeof...(Args)> ArgArray; \
			typename ArgArray::Type array{ \
			ArgArray::template make<fmt::BasicFormatter<Char> >(args)...}; \
			call(FMT_FOR_EACH(FMT_GET_ARG_NAME, __VA_ARGS__), \
				fmt::ArgList(fmt::internal::make_type(args...), array)); \
	}
	
};
*/
Core_API std::string Utf8ToUpperLatin(const std::string& Input);

bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr);

bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str);

bool WStrToUtf8(std::wstring const& wstr, std::string& utf8str);

inline wchar_t wcharToUpper(wchar_t wchar)
{
	if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
		return wchar_t(uint16(wchar) - 0x0020);
	if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
		return wchar_t(0x1E9E);
	if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
		return wchar_t(uint16(wchar) - 0x0020);
	if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
		return wchar_t(uint16(wchar) - 0x0020);
	if (wchar >= 0x0101 && wchar <= 0x012F)                  // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK (only %2=1)
	{
		if (wchar % 2 == 1)
			return wchar_t(uint16(wchar) - 0x0001);
	}
	if (wchar >= 0x0430 && wchar <= 0x044F)                  // CYRILLIC SMALL LETTER A - CYRILLIC SMALL LETTER YA
		return wchar_t(uint16(wchar) - 0x0020);
	if (wchar == 0x0451)                                     // CYRILLIC SMALL LETTER IO
		return wchar_t(0x0401);

	return wchar;
}

inline wchar_t wcharToUpperOnlyLatin(wchar_t wchar)
{
	return 0;// isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
}

Core_API std::string ByteArrayToHexStr(uint8 const* bytes, uint32 arrayLen, bool reverse = false);


Core_API bool Utf8ToUpperOnlyLatin(std::string& utf8String);