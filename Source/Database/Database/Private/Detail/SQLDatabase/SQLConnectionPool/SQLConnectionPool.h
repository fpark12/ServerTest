#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnection.h"

struct SQLConnectionInfo;
class SQLConnection;
class SQLOperation;

class SQLConnectionPool
{

private:
	friend class SQLThread;

	int MaxConnectionPerSchema;
	std::vector<SQLConnectionInfo> AllSchemaConnectionInfo;
	std::vector<SQLConnection*> Connections;
	ProducerConsumerQueue<SQLOperation*> TaskQueue;

public:
	SQLConnectionPool() = default;
	~SQLConnectionPool() = default;

	enum Schema
	{
		Login,
		Character,
		Hotfix,
		World,
		Count
	};

	SQLConnectionPool(int _max_connection_per_schema);

};

static SQLConnectionPool& ConnectionPool = Singleton<SQLConnectionPool>().Instance();