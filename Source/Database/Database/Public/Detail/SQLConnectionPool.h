#pragma once
#include "Private/Detail/SQLConnectionPool/SQLConnection.h"

struct SQLConnectionInfo;
class SQLConnection;
class SQLOperation;

class SQLConnectionPool
{
	DECLARE_SINGLETON(SQLConnectionPool)

private:
	friend class SQLThread;

	int MaxConnectionPerSchema;
	std::vector<SQLConnectionInfo> AllSchemaConnectionInfo;
	std::vector<SQLConnection*> Connections;
	ProducerConsumerQueue<SQLOperation*> TaskQueue;

public:

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