#pragma once

struct SQLConnectionInfo
{
	// port number is included in hostname
	std::string Hostname;
	std::string Username;
	std::string Password;
	std::string Schema;
	uint32 Port;
};

class SQLConnection
{
	friend class SQLOperationBase;
	friend class SQLConnectionPool;

private:

	MYSQL* MySqlHandle;
	std::vector<MYSQL_STMT*> PreparedStatements;
	SQLConnectionInfo ConnectionInfo;
	std::atomic<bool> IsAvaliable;

public:
	
	SQLConnection();
	SQLConnection(SQLConnection&& Other) :
		MySqlHandle(std::move(Other.MySqlHandle)),
		PreparedStatements(std::move(Other.PreparedStatements)),
		ConnectionInfo(std::move(Other.ConnectionInfo)),
		IsAvaliable(std::move(Other.IsAvaliable.load()))
	{
	}
	SQLConnection& operator=(SQLConnection&& Other)
	{
		if (MySqlHandle)
		{
			MySqlHandle = std::move(Other.MySqlHandle);
			PreparedStatements = std::move(Other.PreparedStatements);
		}

		IsAvaliable = std::move(Other.IsAvaliable.load());
		ConnectionInfo = std::move(Other.ConnectionInfo);
		return *this;
	}
	SQLConnection(SQLConnectionInfo& _info);
	~SQLConnection();
	Status Connect();

	virtual Status InitPreparedStatements(std::vector<char*>& preparedStmtStrings);
	
	Status PrepareStatement(char* stmtString);
};