#pragma once
#include "Public/Detail/SQLTask.h"
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnectionPool.h"

class SQLDatabase
{
	DECLARE_SINGLETON(SQLDatabase);

private:
	std::vector<SQLConnectionPool> ConnectionPool;
	std::vector<SQLConnectionPoolInfo> ConnectionPoolInfo;
	moodycamel::ConcurrentQueue<SQLTask*> TaskQueue;

public:

	void AddSchema(uint32 schemaIndex, SQLConnectionPoolInfo& poolInfo);

	SQLConnection* GetAvaliableSQLConnection(uint32 schemaIndex);
	SQLConnectionPool* GetSQLConnectionPool(uint32 schemaIndex);

	Status SpawnSQLConnections();

	Status AddTask(SQLTask* task);

	Status BulkAddTasks(std::vector<SQLTask*>& tasks);

	SQLTask* NextTask();
};

static SQLDatabase& GDatabase = Singleton<SQLDatabase>().Instance();