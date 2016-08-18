#pragma once
#include <mysql.h>

class SQLOperation;
class DatabaseWorker;

struct DatabaseConnectionInfo
{
	// port number is included in hostname
	std::string Hostname;
	std::string Username;
	std::string Password;
	std::string Schema;
	uint32 Port;
};

class SQLConnection
{
private:
	friend class DatabaseWorker;
	friend class SQLOperation;

	MYSQL* MySqlHandle;
	MYSQL_STMT* MySqlStatementHandle;
	DatabaseConnectionInfo ConnectionInfo;

	ProducerConsumerQueue<SQLOperation*>* OperationQueue;

public:
	
	SQLConnection(DatabaseConnectionInfo& _info);
	~SQLConnection();

	void AddTask(SQLOperation* operation);

private:

	uint32 Connect();

	uint32 InitStatement();
	
};