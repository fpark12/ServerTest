#pragma once
#include "SQLThreadPool.h"

class SQLThreadPool
{
	DECLARE_SINGLETON(SQLThreadPool)
private:

public:
	void SpawnThread();
};

static SQLThreadPool& ThreadPool = Singleton<SQLThreadPool>().Instance();