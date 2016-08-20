#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnection.h"

struct SQLConnectionInfo;
class SQLConnection;
class SQLOperation;

template <typename T>
class SQLConnectionPool
{

private:
	friend class SQLThread;

	int MaxConnection;
	SQLConnectionInfo SchemaConnectionInfo;
	std::vector<T*> Connections;
	ProducerConsumerQueue<SQLOperation*> TaskQueue;

public:
	SQLConnectionPool() = default;
	~SQLConnectionPool() = default;
	
	SQLConnectionPool(int _max_connection_per_schema);

};
