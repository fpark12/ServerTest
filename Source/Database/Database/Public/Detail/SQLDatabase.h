#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnectionPool.h"
#include "Private/Detail/SQLDatabase/SQLThreadPool/SQLThreadPool.h"

class SQLDatabase
{
private:
	SQLConnectionPool ConnectionPool;
	SQLThreadPool ThreadPool;

	SQLConnectionInfo ConnectionInfo;
public:
	SQLDatabase() = default;
	SQLDatabase(SQLConnectionInfo& ConnectionInfo)
	{
		this->ConnectionInfo = ConnectionInfo;
	}

	void Connect()
	{
		for (int i = 0; i < ConnectionInfo.ConnectionCount; i++)
		{

		}
	}

	~SQLDatabase() = default;
};