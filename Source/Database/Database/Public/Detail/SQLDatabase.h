#pragma once
#include "Private\Detail\SQLOperation\SQLTask.h"
#include "Private\Detail\SQLDatabase\SQLConnectionPool\SQLConnectionPool.h"

class SQLDatabase
{
	DECLARE_SINGLETON(SQLDatabase);

private:
	std::vector<SQLConnectionPool> ConnectionPool;
	std::vector<SQLConnectionPoolInfo> ConnectionPoolInfo;
	moodycamel::ConcurrentQueue<SQLTask*> TaskQueue;

public:

	void AddSchema(uint32 _index, SQLConnectionPoolInfo& _pool_info);

	SQLConnection* GetAvaliableSQLConnection(uint32 _schema_idx);

	uint32 SpawnSQLConnections();

	uint32 AddTask(SQLTask* _task);

	uint32 BulkAddTasks(std::vector<SQLTask*>& _tasks);

	SQLTask* NextTask();
};

static SQLDatabase& GDatabase = Singleton<SQLDatabase>().Instance();