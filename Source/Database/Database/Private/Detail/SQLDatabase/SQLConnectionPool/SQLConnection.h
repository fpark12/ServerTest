#pragma once

struct SQLConnectionInfo
{
	// port number is included in hostname
	std::string Hostname;
	std::string Username;
	std::string Password;
	std::string Schema;
	uint32 Port;
	uint32 ConnectionCount;
};

class SQLConnection
{
private:
	friend class SQLThread;
	friend class SQLOperation;

	MYSQL* MySqlHandle;
	std::vector<MYSQL_STMT*> PreparedStatementRepository;
	SQLConnectionInfo ConnectionInfo;

public:
	
	SQLConnection(SQLConnectionInfo& _info);
	~SQLConnection();
	uint32 Connect();
	virtual uint32 InitPreparedStatements();
		
};