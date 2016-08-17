#pragma once
#include "spdlog/spdlog.h"

class Logger
{
	DECLARE_SINGLETON(Logger);
private:
	std::shared_ptr<spdlog::logger> Console;
public:
	void Init()
	{
		Console = spdlog::stdout_logger_mt("console", true);
	}


	template <typename... Args>
	inline void Critical(const char* fmt, const Args&... args)
	{
		Console->log(spdlog::level::critical, fmt, args...);
	}

	void Test()
	{
		Console->info("Welcome to spdlog!");
		Console->error("An info message example {}..", 1);

		// Formatting examples
		Console->warn("Easy padding in numbers like {:08d}", 12);
		Console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
		Console->info("Support for floats {:03.2f}", 1.23456);
		Console->info("Positional args are {1} {0}..", "too", "supported");

		Console->info("{:<30}", "left aligned");
		Console->info("{:>30}", "right aligned");
		Console->info("{:^30}", "centered");

		spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name) function");

		// Runtime log levels
		spdlog::set_level(spdlog::level::info); //Set global log level to info
		Console->debug("This message shold not be displayed!");
		Console->set_level(spdlog::level::debug); // Set specific logger's log level
		Console->debug("This message shold be displayed..");

		// Create basic file logger (not rotated)
		auto my_logger = spdlog::basic_logger_mt("basic_logger", "basic.txt");
		my_logger->info("Some log message");


		// Create a file rotating logger with 5mb size max and 3 rotated files
		auto rotating_logger = spdlog::rotating_logger_mt("some_logger_name", "mylogfile", 1048576 * 5, 3);
		for (int i = 0; i < 10; ++i)
			rotating_logger->info("{} * {} equals {:>10}", i, i, i*i);

		// Create a daily logger - a new file is created every day on 2:30am
		auto daily_logger = spdlog::daily_logger_mt("daily_logger", "daily", 2, 30);
		daily_logger->info(123.44);

		// Customize msg format for all messages
		spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
		rotating_logger->info("This is another message with custom format");

		// Compile time debug or trace macros.
		// Enabled #ifdef SPDLOG_DEBUG_ON or #ifdef SPDLOG_TRACE_ON
		SPDLOG_TRACE(Console, "Enabled only #ifdef SPDLOG_TRACE_ON..{} ,{}", 1, 3.23);
		SPDLOG_DEBUG(Console, "Enabled only #ifdef SPDLOG_DEBUG_ON.. {} ,{}", 1, 3.23);
	}
};

