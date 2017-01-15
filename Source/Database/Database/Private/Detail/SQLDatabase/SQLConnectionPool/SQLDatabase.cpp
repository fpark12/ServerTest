#include "Public\Detail\SQLDatabase.h"

void SQLDatabase::AddSchema(uint32 _index, SQLConnectionPoolInfo& _pool_info)
{
	if (ConnectionPool.size() <= _index)
	{
		ConnectionPool.resize(_index + 1);
		ConnectionPool[_index] = std::move(SQLConnectionPool(_pool_info));
	}
}

SQLConnection* SQLDatabase::GetAvaliableSQLConnection(uint32 _schema_idx)
{
	return ConnectionPool[_schema_idx].GetAvaliableSQLConnection();
}

uint32 SQLDatabase::SpawnSQLConnections()
{
	for (auto& pool : ConnectionPool)
	{
		if (pool.SpawnConnections())
		{
			//TODO Error Handling
			return RC_FAILED;
		}
	}
	return RC_SUCCESS;
}

uint32 SQLDatabase::AddTask(SQLTask* _task)
{
	if (!TaskQueue.try_enqueue(_task))
	{
		//TODO Error handling
		return RC_FAILED;
	}
	return RC_SUCCESS;
}

uint32 SQLDatabase::BulkAddTasks(std::vector<SQLTask*>& _tasks)
{
	if (!
		TaskQueue.try_enqueue_bulk(std::make_move_iterator(_tasks.begin()),
			_tasks.size())
		)
	{
		//TODO Error handling
		return RC_FAILED;
	}
	return RC_SUCCESS;
}

SQLTask* SQLDatabase::NextTask()
{
	SQLTask* next = nullptr;
	if (!TaskQueue.try_dequeue(next))
	{
		//TODO Error handling
		return nullptr;
	}
	return next;
}

