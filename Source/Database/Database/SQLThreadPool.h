#pragma once

class SQLThreadPool
{
	DECLARE_SINGLETON(SQLThreadPool)
private:

public:

};

static SQLThreadPool& ThreadPool = Singleton<SQLThreadPool>().Instance();