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

bool Utf8ToUpperOnlyLatin(std::string& utf8String)
{
	std::wstring wstr;
	if (!Utf8toWStr(utf8String, wstr))
		return false;

	std::transform(wstr.begin(), wstr.end(), wstr.begin(), wcharToUpperOnlyLatin);

	return WStrToUtf8(wstr, utf8String);
}