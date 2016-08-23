#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnectionPool.h"
#include "Private/Detail/SQLDatabase/SQLThreadPool/SQLThreadPool.h"
#include "Public/Detail/SQLOperation.h"

class SQLDatabase
{
	DECLARE_SINGLETON(SQLDatabase);

private:
	std::vector<SQLConnectionPool> ConnectionPool;
	std::vector<SQLConnectionPoolInfo> ConnectionPoolInfo;
	ProducerConsumerQueue<SQLOperation> TaskQueue;
public:

	SQLDatabase(std::vector<SQLConnectionPoolInfo>& infoList) :
		ConnectionPoolInfo(infoList)
	{
		ConnectionPool.reserve(infoList.size());
		for (auto& info: infoList)
		{
			ConnectionPool.push_back(SQLConnectionPool(info));
		}
	}

	uint32 InitConnection()
	{
		for (auto& pool: ConnectionPool)
		{
			if (pool.InitConnection())
			{
				//TODO Error Handling
			}
		}
		return 0;
	}

	void Dispatch()
	{

	}
};