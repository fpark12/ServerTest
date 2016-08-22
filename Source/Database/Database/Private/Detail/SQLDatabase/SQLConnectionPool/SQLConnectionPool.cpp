#include "SQLConnectionPool.h"

SQLConnectionPool::SQLConnectionPool(SQLConnectionPoolInfo& info) : ConnectionPoolInfo(info)
{

}

uint32 SQLConnectionPool::InitConnection()
{
	Connections.resize(ConnectionPoolInfo.ConnectionCount);
	for (int i = 0; i < ConnectionPoolInfo.ConnectionCount; ++i)
	{
		Connections[i] = SQLConnection(SQLConnectionInfo(ConnectionPoolInfo));
		if (Connections[i].Connect())
		{
			//TODO Error handling
		}
		
		if (Connections[i].InitPreparedStatements(PreparedStatementStrings))
		{
			//TODO Error handling
		}
	}
	return 0;
}

