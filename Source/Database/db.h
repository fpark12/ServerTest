#pragma once
// #include <deque>
#include <mysql.h>
#include <string>

class DatabaseConnection;
// class Task;

class dbOperation
{
public:
	// single thread db operation
	dbOperation(DatabaseConnection* _dbConn);

	// multi thread db operation
	// dbOperation(DatabaseConnection* dbConn, std::deque<Task> taskQueue);

	bool connect();

	int query(std::string sqlStatement);

	bool insert(std::string sqlStatement);
private:
	DatabaseConnection* dbConn;
	MYSQL* connection;
	MYSQL_RES *res_set;
	MYSQL_ROW row;
};