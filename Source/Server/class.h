#pragma once
#include <string>

class Functor
{
public:
	void operator()();

	void operator()(std::string& msg);
};