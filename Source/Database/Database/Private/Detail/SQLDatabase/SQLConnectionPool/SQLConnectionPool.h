#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnection.h"

struct SQLConnectionInfo;
class SQLConnection;
class SQLOperationBase;


struct SQLConnectionPoolInfo : SQLConnectionInfo
{
	static const int32 InvalidConnectionPool = -1;
	uint32 ConnectionCount;
};

class SQLConnectionPool
{

private:

	SQLConnectionPoolInfo ConnectionPoolInfo;
	uint32 ActiveConnectionCount;
	std::vector<SQLConnection> Connections;
	// won't compile std::vector<std::atomic<bool>> IsConectionInUse;
	std::vector<char*> PreparedStatementStrings;

public:

	SQLConnectionPool();
	SQLConnectionPool(SQLConnectionPool&&) = default;
	SQLConnectionPool& operator=(SQLConnectionPool&&) = default;

	SQLConnectionPool(SQLConnectionPoolInfo& info);
	~SQLConnectionPool() = default;

	uint32 SpawnConnections();

	SQLConnection* GetAvaliableSQLConnection();
};
