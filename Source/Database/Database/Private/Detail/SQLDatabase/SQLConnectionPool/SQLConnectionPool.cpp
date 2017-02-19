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

Status SQLConnectionPool::SpawnConnections()
{
	if (ConnectionPoolInfo.ConnectionCount <= 0)
	{
		return SC::OK;
	}
	if (ActiveConnectionCount != ConnectionPoolInfo.ConnectionCount)
	{
		Connections.reserve(ConnectionPoolInfo.ConnectionCount);
		for (int i = ActiveConnectionCount; i < ConnectionPoolInfo.ConnectionCount; ++i)
		{
			Connections.emplace_back(SQLConnection(ConnectionPoolInfo));
			if (SC::OK != Connections[i].Connect())
			{
				//TODO Error handling
				return SC::FAILED;
			}
		
			if (SC::OK != Connections[i].InitPreparedStatements(PreparedStatementStrings))
			{
				//TODO Error handling
				return SC::FAILED;
			}
		}
		ActiveConnectionCount = ConnectionPoolInfo.ConnectionCount;
	}
	
	return SC::OK;
}

SQLConnection* SQLConnectionPool::GetAvaliableSQLConnection()
{
	bool Expected = true;
	for (auto& conn : Connections)
	{
		Expected = true;
		if (conn.IsAvaliable.compare_exchange_strong(Expected, false))
		{
			return &conn;
		}
	}
	return nullptr;
}

