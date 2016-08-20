#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnection.h"

struct SQLConnectionInfo;
class SQLConnection;
class SQLOperation;


struct SQLConnectionPoolInfo : SQLConnectionInfo
{
	int ConnectionCount;
};

class SQLConnectionPool
{

private:
	friend class SQLThread;

	SQLConnectionPoolInfo ConnectionPoolInfo;
	std::vector<SQLConnection> Connections;
	std::vector<char*> PreparedStatementStrings;

public:
	SQLConnectionPool(SQLConnectionPoolInfo& info);
	~SQLConnectionPool() = default;

	void Init();
};
