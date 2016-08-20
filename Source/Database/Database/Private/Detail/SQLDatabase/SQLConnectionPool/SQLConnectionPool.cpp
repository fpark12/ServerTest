#include "SQLConnectionPool.h"

SQLConnectionPool::SQLConnectionPool(SQLConnectionPoolInfo& info) : ConnectionPoolInfo(info)
{

}

void SQLConnectionPool::Init()
{
	Connections.resize(ConnectionPoolInfo.ConnectionCount);
	for (int i = 0; i < ConnectionPoolInfo.ConnectionCount; ++i)
	{
		Connections[i] = SQLConnection(SQLConnectionInfo(ConnectionPoolInfo));
		if (Connections[i].Connect())
		{
			// Error handling
		}
		
		if (Connections[i].InitPreparedStatements(PreparedStatementStrings))
		{
			// Error handling
		}
	}
}

