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

public:
	
	DatabaseConnection(DatabaseConnectionInfo& _info);

	bool Connect();

private:
	MYSQL* MySqlHandle;
	DatabaseConnectionInfo ConnectionInfo;

	ProducerConsumerQueue<SQLOperation*>* OperationQueue;
	
};