#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnectionPool.h"
#include "Public/Detail/SQLOperation.h"

class SQLDatabase
{
	DECLARE_SINGLETON(SQLDatabase);

private:
	std::vector<SQLConnectionPool> ConnectionPool;
	std::vector<SQLConnectionPoolInfo> ConnectionPoolInfo;
	moodycamel::ConcurrentQueue<SQLOperation*> TaskQueue;

public:

	void AddSchema(uint32 index, SQLConnectionPoolInfo& poolInfo)
	{
		if (ConnectionPool.size() <= index)
		{
			ConnectionPool.resize(index + 1);
			ConnectionPool[index] = SQLConnectionPool(poolInfo);
		}
	}

	SQLConnection* GetFreeConnectionByType(uint32 index)
	{
		return ConnectionPool[index].GetFreeConnection();
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