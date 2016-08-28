#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnectionPool.h"
#include "Public/Detail/SQLOperation.h"


enum SQLThreadingMode
{
	Shared = 0,
	Dedicated = 1
};
struct SQLSchemaInfo : SQLConnectionPoolInfo
{
	uint32 ThreadingMode;
};

class SQLDatabase
{
	DECLARE_SINGLETON(SQLDatabase);

private:
	std::vector<SQLConnectionPool> ConnectionPool;
	std::vector<SQLConnectionPoolInfo> ConnectionPoolInfo;
	moodycamel::ConcurrentQueue<SQLOperation*> TaskQueue;

public:

	void AddSchema(uint32 index, SQLSchemaInfo& poolInfo)
	{
		if (ConnectionPool.size() <= index)
		{
			ConnectionPool.resize(index + 1);
			ConnectionPool[index] = std::move(SQLConnectionPool(poolInfo));
		}
	}

	SQLConnection* GetFreeSQLConnection(uint32 index)
	{
		return ConnectionPool[index].GetFreeSQLConnection();
	}

	uint32 SpawnSQLConnections()
	{
		for (auto& pool: ConnectionPool)
		{
			if (pool.SpawnConnections())
			{
				//TODO Error Handling
			}
		}
		return RC_SUCCESS;
	}

	uint32 AddTask(SQLOperation* operation)
	{
		if (!TaskQueue.try_enqueue(operation))
		{
			//TODO Error handling
			return RC_FAILED;
		}
		return RC_SUCCESS;
	}

	uint32 BulkAddTasks(std::vector<SQLOperation*>& operations)
	{
		if (!
			TaskQueue.try_enqueue_bulk(std::make_move_iterator(operations.begin()), 
				operations.size())
			)
		{
			//TODO Error handling
			return RC_FAILED;
		}
		return RC_SUCCESS;
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