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
	moodycamel::ConcurrentQueue<SQLOperation*> TaskQueue;
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

	uint32 AddTask(SQLOperation* operation)
	{
		if (!TaskQueue.try_enqueue(operation))
		{
			//TODO Error handling
			return 1;
		}
		return 0;
	}

	uint32 BulkAddTasks(std::vector<SQLOperation*>& operations)
	{
		if (!
			TaskQueue.try_enqueue_bulk(std::make_move_iterator(operations.begin()), 
				operations.size())
			)
		{
			//TODO Error handling
			return 1;
		}
		return 0;
	}

	SQLOperation* NextTask()
	{
		SQLOperation* next = nullptr;
		if (!TaskQueue.try_dequeue(next))
		{
			//TODO Error handling
			return nullptr;
		}
		return next;
	}
};

static SQLDatabase& GDatabase = Singleton<SQLDatabase>().Instance();