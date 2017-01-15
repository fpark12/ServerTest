#include "StringUtils.h"

std::string Utf8ToUpperLatin(const std::string& Input)
{
	setlocale(LC_CTYPE, "en_US.UTF8");
	auto& f = std::use_facet<std::ctype<char>>(std::locale());
	std::string Result = Input;
	f.toupper(&Result[0], &Result[0] + Result.size());
	return Result;
}

bool Utf8toWStr(const std::string & utf8str, std::wstring & wstr)
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

bool WStrToUtf8(wchar_t * wstr, size_t size, std::string & utf8str)
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

bool WStrToUtf8(std::wstring const & wstr, std::string & utf8str)
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

bool Utf8ToUpperOnlyLatin(std::string & utf8String)
{
	std::wstring wstr;
	//if (!Utf8toWStr(utf8String, ,wstr))
	//	return false;

	std::transform(wstr.begin(), wstr.end(), wstr.begin(), wcharToUpperOnlyLatin);

	return WStrToUtf8(wstr, utf8String);
}
