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
	void Init()
	{
		auto console = spdlog::stdout_logger_mt("console", true);
		console->info("Welcome to spdlog!");
		console->error("An info message example {}..", 1);

		// Formatting examples
		console->warn("Easy padding in numbers like {:08d}", 12);
		console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
		console->info("Support for floats {:03.2f}", 1.23456);
		console->info("Positional args are {1} {0}..", "too", "supported");

		console->info("{:<30}", "left aligned");
		console->info("{:>30}", "right aligned");
		console->info("{:^30}", "centered");

		spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name) function");

		// Runtime log levels
		spdlog::set_level(spdlog::level::info); //Set global log level to info
		console->debug("This message shold not be displayed!");
		console->set_level(spdlog::level::debug); // Set specific logger's log level
		console->debug("This message shold be displayed..");

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
		SPDLOG_TRACE(console, "Enabled only #ifdef SPDLOG_TRACE_ON..{} ,{}", 1, 3.23);
		SPDLOG_DEBUG(console, "Enabled only #ifdef SPDLOG_DEBUG_ON.. {} ,{}", 1, 3.23);
	}
	static Log& Instance()
	{
		static Log Instance;
		return Instance;
	}
};

#define GLog GLogRef
static Log& GLogRef = Log::Instance();