#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnection.h"

struct SQLConnectionInfo;
class SQLConnection;
class SQLOperation;


struct SQLConnectionPoolInfo : SQLConnectionInfo
{
	uint32 ConnectionCount;
};

class SQLConnectionPool
{

private:
	friend class SQLThread;

	SQLConnectionPoolInfo ConnectionPoolInfo;
	std::vector<SQLConnection> Connections;
	// won't compile std::vector<std::atomic<bool>> IsConectionInUse;
	std::vector<char*> PreparedStatementStrings;

public:
	SQLConnectionPool(SQLConnectionPoolInfo& info);
	~SQLConnectionPool() = default;

	uint32 InitConnection();

	SQLConnection* GetFreeConnection()
	{

	}
};
