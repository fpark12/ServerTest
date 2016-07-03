#include "DatabaseWorker.h"


bool DatabaseWorker::OpenConnection()
{
	MysqlConnection = mysql_init(NULL);

	if (!MysqlConnection)
	{
		//TODO Error Log
		return false;
	}

	MysqlConnection = mysql_real_connect(
		MysqlConnection,
		ConnectionCreateInfo->Hostname.c_str(),
		ConnectionCreateInfo->Username.c_str(),
		ConnectionCreateInfo->Password.c_str(),
		ConnectionCreateInfo->Schema.c_str(),
		0, NULL, 0);

	if (!connect)
	{
		//TODO Error Log
		return false;
	}

	return true;
}

bool DatabaseWorker::CloseConnection()
{
	mysql_close(MysqlConnection);
	return true;
}

MYSQL* DatabaseWorker::GetConnection()
{
	return MysqlConnection;
}

