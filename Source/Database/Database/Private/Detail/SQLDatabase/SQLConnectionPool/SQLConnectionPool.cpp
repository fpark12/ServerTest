#include "SQLConnectionPool.h"

SQLConnectionPool::SQLConnectionPool(int _max_connection_per_schema) :
	MaxConnectionPerSchema(_max_connection_per_schema)
{
	// Connections.reserve(Schema::Count * MaxConnectionPerSchema);
}

