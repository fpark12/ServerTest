#pragma once
#include "spdlog/spdlog.h"

class Log
{
private:
	Log() = default;
	~Log() = default;
	Log(Log const&) = delete;
	Log& operator=(Log const&) = delete;

public:
	static Log& Instance()
	{
		static Log Instance;
		return Instance;
	}
};