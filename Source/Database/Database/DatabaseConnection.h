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

class DatabaseConnection
{
private:
	friend class DatabaseWorker;
	friend class SQLOperation;

	MYSQL* MySqlHandle;
	MYSQL_STMT* MySqlStatementHandle;
	DatabaseConnectionInfo ConnectionInfo;

	ProducerConsumerQueue<SQLOperation*>* OperationQueue;

public:
	
	DatabaseConnection(DatabaseConnectionInfo& _info);
	~DatabaseConnection();

	void AddTask(SQLOperation* operation);

private:

	uint32 Connect();

	uint32 InitStatement();
	
};