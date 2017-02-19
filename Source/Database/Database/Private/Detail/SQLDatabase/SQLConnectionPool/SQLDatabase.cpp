#include "Public\Detail\SQLDatabase.h"

void SQLDatabase::AddSchema(uint32 schemaIndex, SQLConnectionPoolInfo& poolInfo)
{
	if (ConnectionPool.size() <= schemaIndex)
	{
		ConnectionPool.resize(schemaIndex + 1);
		ConnectionPool[schemaIndex] = std::move(SQLConnectionPool(poolInfo));
	}
}

SQLConnection* SQLDatabase::GetAvaliableSQLConnection(uint32 schemaIndex)
{
	return ConnectionPool[schemaIndex].GetAvaliableSQLConnection();
}

SQLConnectionPool * SQLDatabase::GetSQLConnectionPool(uint32 schemaIndex)
{
	return &ConnectionPool[schemaIndex];
}

Status SQLDatabase::SpawnSQLConnections()
{
	for (auto& pool : ConnectionPool)
	{
		if (Status::OK != pool.SpawnConnections())
		{
			//TODO Error Handling
			return Status::FAILED;
		}
	}
	return Status::OK;
}

Status SQLDatabase::AddTask(SQLTask* task)
{
	if (!TaskQueue.try_enqueue(task))
	{
		//TODO Error handling
		return Status::FAILED;
	}
	return Status::OK;
}

Status SQLDatabase::BulkAddTasks(std::vector<SQLTask*>& task)
{
	if (!
		TaskQueue.try_enqueue_bulk(std::make_move_iterator(task.begin()),
			task.size())
		)
	{
		//TODO Error handling
		return Status::FAILED;
	}
	return Status::OK;
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

