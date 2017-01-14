#pragma once
#include "utf8.h"

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
	*/
	//
};

bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr)
{
	try
	{
		if (size_t len = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size()))
		{
			wstr.resize(len);
			utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
		}
	}
	catch (std::exception)
	{
		wstr.clear();
		return false;
	}

	return true;
}

bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str)
{
	try
	{
		std::string utf8str2;
		utf8str2.resize(size * 4);                            // allocate for most long case

		if (size)
		{
			char* oend = utf8::utf16to8(wstr, wstr + size, &utf8str2[0]);
			utf8str2.resize(oend - (&utf8str2[0]));               // remove unused tail
		}
		utf8str = utf8str2;
	}
	catch (std::exception)
	{
		utf8str.clear();
		return false;
	}

	return true;
}

bool WStrToUtf8(std::wstring const& wstr, std::string& utf8str)
{
	try
	{
		std::string utf8str2;
		utf8str2.resize(wstr.size() * 4);                     // allocate for most long case

		if (wstr.size())
		{
			char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str2[0]);
			utf8str2.resize(oend - (&utf8str2[0]));                // remove unused tail
		}
		utf8str = utf8str2;
	}
	catch (std::exception)
	{
		utf8str.clear();
		return false;
	}

	return true;
}

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
	return isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
}


bool Utf8ToUpperOnlyLatin(std::string& utf8String)
{
	std::wstring wstr;
	if (!Utf8toWStr(utf8String, ,wstr))
		return false;

	std::transform(wstr.begin(), wstr.end(), wstr.begin(), wcharToUpperOnlyLatin);

	return WStrToUtf8(wstr, utf8String);
}