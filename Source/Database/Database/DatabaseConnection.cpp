#include "DatabaseConnection.h"
#include "DatabaseWorker.h"

DatabaseConnection::DatabaseConnection(DatabaseConnectionInfo& _info) : 
	ConnectionInfo(_info),
	OperationQueue(new ProducerConsumerQueue<SQLOperation*>())
{
	if (Connect())
	{
		//TODO error log
	}

	if (InitStatement())
	{
		//TODO error log
	}
}

DatabaseConnection::~DatabaseConnection()
{
	if (mysql_stmt_close(MySqlStatementHandle))
	{
		// fprintf(stderr, " failed while closing the statement\n");
		// fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		// TODO error code
		exit(EXIT_FAILURE);
	}

	mysql_close(MySqlHandle);
}

void DatabaseConnection::AddTask(SQLOperation* operation)
{
	OperationQueue->Push(operation);
}

uint32 DatabaseConnection::Connect()
{
	MYSQL* initMysql = mysql_init(NULL);
	if (!initMysql)
	{
		// TODO Error Log: not enough memory to spawn a new mysql connection handle
		return 1;
	}

	mysql_options(initMysql, MYSQL_SET_CHARSET_NAME, "utf8");

	MySqlHandle = mysql_real_connect(initMysql, ConnectionInfo.Hostname.c_str(),
		ConnectionInfo.Username.c_str(), ConnectionInfo.Password.c_str(),
		ConnectionInfo.Schema.c_str(), ConnectionInfo.Port, NULL, 0);

	if (MySqlHandle)
	{
		// TODO Logger: Get library & server version info
		// TC_LOG_INFO("sql.sql", "MySQL client library: %s", mysql_get_client_info());
		// TC_LOG_INFO("sql.sql", "MySQL server ver: %s ", mysql_get_server_info(m_Mysql));

		// TODO Logger: Success message TC_LOG_INFO("sql.sql", "Connected to MySQL database at %s", m_connectionInfo.host.c_str());

		mysql_autocommit(MySqlHandle, 1); //Turn on auto commit mode

		mysql_set_character_set(MySqlHandle, "utf8");
		return 0;
	}
	else
	{
		// TODO Error Log: unsuccess message TC_LOG_ERROR("sql.sql", "Could not connect to MySQL database at %s: %s", m_connectionInfo.host.c_str(), mysql_error(mysqlInit));
		// mysql_errno(mysqlInit)
		uint32 error_code = mysql_errno(initMysql);
		mysql_close(initMysql);
		return error_code;
	}
}

uint32 DatabaseConnection::InitStatement()
{
	MySqlStatementHandle = mysql_stmt_init(MySqlHandle);
	if (!MySqlStatementHandle)
	{
		//TODO error log
		return mysql_errno(MySqlHandle);
	}
	return 0;
}

