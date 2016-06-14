#include "db.h"
#include "database.pb.h"

dbOperation::dbOperation(DatabaseConnection* _dbConn)
{
	dbConn = _dbConn;
	connection = mysql_init(nullptr);
}

bool dbOperation::connect()
{
	connection = mysql_real_connect(connection, dbConn->database_ip().c_str(), dbConn->db_username().c_str(), dbConn->db_password().c_str(), dbConn->db_schema().c_str(), 0, NULL, 0);
	return true;
}

int dbOperation::query(std::string sqlStatement)
{
	mysql_query(connection, sqlStatement.c_str());
	res_set = mysql_store_result(connection);
	return 0;
}

bool dbOperation::insert(std::string sqlStatement)
{
	return true;
}