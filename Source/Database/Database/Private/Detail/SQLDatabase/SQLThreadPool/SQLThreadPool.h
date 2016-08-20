#pragma once
#include "SQLThreadPool.h"

class SQLThreadPool
{
private:

public:
	void SpawnThread();
	SQLThreadPool() = default;
	~SQLThreadPool() = default;
};

//static SQLThreadPool& ThreadPool = Singleton<SQLThreadPool>().Instance();