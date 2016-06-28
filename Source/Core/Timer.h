#pragma once
#include <chrono>
#include "Platform.h"

inline uint32 getMSTime()
{
	using namespace std::chrono;

	static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

	return uint32(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
}