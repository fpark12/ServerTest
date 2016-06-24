#pragma once
#include <mysql.h>
#include <thread>
#include <string>
#include "Define.h"

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

	bool OpenConnection()
	{
		MysqlConnection = mysql_init(NULL);

		if (!MysqlConnection)
		{
			//TODO Error Log
			return false;
		}

		MysqlConnection = mysql_real_connect(
			MysqlConnection,
			ConnectionCreateInfo->Hostname.c_str(),
			ConnectionCreateInfo->Username.c_str(),
			ConnectionCreateInfo->Password.c_str(),
			ConnectionCreateInfo->Schema.c_str(),
			0, NULL, 0);

		if (!connect)
		{
			//TODO Error Log
			return false;
		}

		return true;
	}

	bool CloseConnection()
	{
		mysql_close(MysqlConnection);
		return true;
	}

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