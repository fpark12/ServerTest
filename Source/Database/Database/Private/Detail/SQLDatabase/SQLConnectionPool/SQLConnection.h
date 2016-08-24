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
	friend class SQLOperation;
	friend class SQLConnectionPool;

private:

	MYSQL* MySqlHandle;
	std::vector<MYSQL_STMT*> PreparedStatements;
	SQLConnectionInfo ConnectionInfo;
	std::atomic_bool IsFree;

public:
	
	SQLConnection();
	SQLConnection(SQLConnection&&) = default;
	SQLConnection& operator=(SQLConnection&&) = default;
	SQLConnection(SQLConnectionInfo& _info);
	~SQLConnection();
	uint32 Connect();

	virtual uint32 InitPreparedStatements(std::vector<char*>& preparedStmtStrings);
	
	uint32 PrepareStatement(char* stmtString);
};