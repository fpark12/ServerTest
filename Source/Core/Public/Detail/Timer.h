#pragma once

inline uint64 getMSTime()
{
	using namespace std::chrono;

	static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

	return uint64(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
}