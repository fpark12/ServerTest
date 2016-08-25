#include "SQLConnectionPool.h"

SQLConnectionPool::SQLConnectionPool()
{
	ConnectionPoolInfo.ConnectionCount = SQLConnectionPoolInfo::InvalidConnectionPool;
}

SQLConnectionPool::SQLConnectionPool(SQLConnectionPoolInfo& info) : ConnectionPoolInfo(info)
{

}

uint32 SQLConnectionPool::InitConnection()
{
	if (ConnectionPoolInfo.ConnectionCount > 0)
	{
		Connections.resize(ConnectionPoolInfo.ConnectionCount);
		for (int i = 0; i < ConnectionPoolInfo.ConnectionCount; ++i)
		{
			Connections[i] = ((SQLConnection &&) std::move(SQLConnection(ConnectionPoolInfo)));
			if (Connections[i].Connect())
			{
				//TODO Error handling
			}
		
			if (Connections[i].InitPreparedStatements(PreparedStatementStrings))
			{
				//TODO Error handling
			}
		}
	}
	
	return 0;
}

SQLConnection* SQLConnectionPool::GetFreeConnection()
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

