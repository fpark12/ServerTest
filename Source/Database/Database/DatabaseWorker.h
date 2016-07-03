#pragma once
#include <mysql.h>
#include <thread>
#include <string>
#include "Define.h"

class PreparedStatement;

struct SchemaAccessInfo
{
	std::string Hostname;
	std::string Username;
	std::string Password;
	std::string Schema;
};
class TaskResultPairQueue;

class DatabaseWorker
{
public:
	DatabaseWorker();
	DatabaseWorker(SchemaAccessInfo* _connInfo, TaskResultPairQueue* taskQueue);
	~DatabaseWorker();

	bool OpenConnection();

	bool CloseConnection();

	MYSQL* GetConnection();

private:
	bool DoTask(PreparedStatement* statement);
	void Work();

private:
	SchemaAccessInfo* ConnectionCreateInfo;
	MYSQL* MysqlConnection;
	TaskResultPairQueue* taskQueue;
	MYSQL_STMT* Stmt;
	MYSQL_BIND* Bind;
	std::thread WorkerThread;

	DISALLOW_COPY(DatabaseWorker);
};