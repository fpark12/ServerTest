#include "SQLConnectionPool.h"

SQLConnectionPool::SQLConnectionPool() :
ActiveConnectionCount(0)
{
	ConnectionPoolInfo.ConnectionCount = SQLConnectionPoolInfo::InvalidConnectionPool;
}

SQLConnectionPool::SQLConnectionPool(SQLConnectionPoolInfo& info) :
	ActiveConnectionCount(0),
	ConnectionPoolInfo(info)
{

}

uint32 SQLConnectionPool::SpawnConnections()
{
	if (ConnectionPoolInfo.ConnectionCount <= 0)
	{
		return RC_SUCCESS;
	}
	if (ActiveConnectionCount != ConnectionPoolInfo.ConnectionCount)
	{
		Connections.reserve(ConnectionPoolInfo.ConnectionCount);
		for (int i = ActiveConnectionCount; i < ConnectionPoolInfo.ConnectionCount; ++i)
		{
			Connections.emplace_back(SQLConnection(ConnectionPoolInfo));
			if (Connections[i].Connect())
			{
				//TODO Error handling
			}
		
			if (Connections[i].InitPreparedStatements(PreparedStatementStrings))
			{
				//TODO Error handling
			}
		}
		ActiveConnectionCount = ConnectionPoolInfo.ConnectionCount;
	}
	
	return RC_SUCCESS;
}

SQLConnection* SQLConnectionPool::GetFreeSQLConnection()
{
	bool Expected = true;
	for (auto& conn : Connections)
	{
		Expected = true;
		if (conn.IsFree.compare_exchange_strong(Expected, false))
		{
			return &conn;
		}
	}
	return nullptr;
}

