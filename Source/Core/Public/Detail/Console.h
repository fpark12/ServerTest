#pragma once
#include "spdlog/spdlog.h"

class Console
{
	DECLARE_SINGLETON(Console);
private:
	std::shared_ptr<spdlog::logger> ConsoleLogger;
public:
	void Init()
	{
		ConsoleLogger = spdlog::stdout_logger_mt("Console", true);
	}

	template <typename... Args>
	inline void Message(const char* fmt, const Args&... args)
	{
		ConsoleLogger->log(spdlog::level::critical, fmt, args...);
	}
	template <typename... Args>
	inline void Print(const char* fmt, const Args&... args)
	{
		ConsoleLogger->log(spdlog::level::critical, fmt, args...);
	}
};