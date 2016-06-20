#pragma once
#include <mysql.h>
#include <thread>
#include "Define.h"

class SchemaAccessInfo;
class TaskResultPairQueue;

class DatabaseWorker
{
public:
	DatabaseWorker();
	DatabaseWorker(SchemaAccessInfo* _connInfo, TaskResultPairQueue* taskQueue);
	~DatabaseWorker();

	bool OpenConnection();
	bool CloseConnection();

	MYSQL* GetConnection() { return MysqlConnection; }

private:
	bool DoTask();
	void Work();

private:
	SchemaAccessInfo* ConnectionCreateInfo;
	MYSQL* MysqlConnection;
	TaskResultPairQueue* taskQueue;
	MYSQL_STMT* Stmt;
	MYSQL_BIND* Bind;
	std::thread WorkerThread;

	DatabaseWorker(DatabaseWorker const& right) = delete;
	DatabaseWorker& operator=(DatabaseWorker const& right) = delete;
};