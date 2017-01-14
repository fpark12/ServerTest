#pragma once
#include "spdlog/spdlog.h"

class Console
{
	DECLARE_SINGLETON(Console);
private:
	std::shared_ptr<spdlog::logger> ConsoleLoggerImpl;
public:
	void Init()
	{
		ConsoleLoggerImpl = spdlog::stdout_color_mt("Console");
	}

	template <typename... Args>
	inline void Message(const char* fmt, const Args&... args)
	{
		ConsoleLoggerImpl->log(spdlog::level::info, fmt, args...);
	}
	template <typename... Args>
	inline void Print(const char* fmt, const Args&... args)
	{
		ConsoleLoggerImpl->log(spdlog::level::info, fmt, args...);
	}
};