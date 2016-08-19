#pragma once
#include "fmt.h"
#include <string>
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