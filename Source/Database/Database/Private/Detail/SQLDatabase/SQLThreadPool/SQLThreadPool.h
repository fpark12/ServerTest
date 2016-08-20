#pragma once
#include "SQLThread.h"

class SQLThreadPool
{
	DECLARE_SINGLETON(SQLThreadPool)

private:
	int MaxThreadCount;
	std::vector<SQLThread*> Threads;

public:
	void SpawnThread();
	SQLThreadPool() = default;
	~SQLThreadPool() = default;
	void SetMaxThread(int maxThread) { MaxThreadCount = maxThread; }
	void SetDefaultMaxThread()
	{
		MaxThreadCount = std::thread::hardware_concurrency();
	}
};

//static SQLThreadPool& ThreadPool = Singleton<SQLThreadPool>().Instance();