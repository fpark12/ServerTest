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

uint32 SQLConnectionPool::InitConnection()
{
	if (ConnectionPoolInfo.ConnectionCount <= 0)
	{
		return 0;
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
	
	return 0;
}

SQLConnection* SQLConnectionPool::GetFreeSQLConnection()
{
	for (auto& conn : Connections)
	{
		if (conn.IsFree)
		{
			return &conn;
		}
	}
	return nullptr;
}

