#pragma once
#include "Private/Detail/SQLDatabase/SQLThreadPool/SQLThread.h"

// Shared Thread: one thread that could serve operations on any schema
// Dedicated Thread: one thread that could only serve operations on one schema

class SQLThreadPool
{
	DECLARE_SINGLETON(SQLThreadPool);

private:
	int SharedThreadCount;
	std::vector<std::unique_ptr<SQLThread>> SharedThreads;
	std::vector<std::vector<std::unique_ptr<SQLThread>>> DedicatedThreads;
public:
	void SpawnThreads();
	void SetThreadCount(int ThreadCount) { this->SharedThreadCount = ThreadCount; }
	void SetDefaultThreadCount()
	{
		SharedThreadCount = std::thread::hardware_concurrency();
	}
};

static SQLThreadPool& GSQLThreadPool = Singleton<SQLThreadPool>().Instance();