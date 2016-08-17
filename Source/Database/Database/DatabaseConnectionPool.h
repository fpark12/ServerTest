#pragma once

struct DatabaseConnectionInfo;
class ResultSet;
class PreparedStatement;

template <class T>
class DatabaseConnectionPool
{
private:
	static int a;
	enum ConnectionType
	{
		IDX_ASYNC,
		IDX_SYNCH,
		IDX_SIZE
	};

	std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> OperationQueue;
	std::array<std::vector<std::unique_ptr<T>>, IDX_SIZE> Connections;
	std::unique_ptr<DatabaseConnectionInfo> ConnectionInfo;
	uint8 AsyncThreadCount;
	uint8 SynchThreadCount;

public:

	/*
	std::future<ResultSet> SubmitAsyncQuery(PreparedStatement* stmt)
	{

	}

	ResultSet SubmitSynchQuery()
	{

	}
	*/
};