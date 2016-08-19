#pragma once
#include "fmt.h"
#include <string>
class CustomString : public std::string
{
	CustomString(fmt::CStringRef format_str, fmt::ArgList args) :
	std::string(fmt::format(format_str, args))
	{
	}
	//
};