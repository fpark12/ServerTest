// Include Header Files
#include <iostream>
#include <cstdio>
#include <cstdlib>

// For MySQL Connection
#include <mysql.h>

// capn proto
#include "Time.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>

#include "Bind.h"

using namespace std;

// Defining Constant Variables
#define SERVER "localhost"
#define USER "root"
#define PASSWORD "Keathalin21"
#define DATABASE "test"

void writeDate(int fd)
{
	::capnp::MallocMessageBuilder message;

	Time::Builder timeBuilder = message.initRoot<Time>();
	timeBuilder.setDay(1);
	timeBuilder.setMonth(1);
	timeBuilder.setYear(2016);

	writePackedMessageToFd(fd, message);
}

void printDate(int fd)
{
	::capnp::PackedFdMessageReader message(fd);
	Time::Reader timeReader = message.getRoot<Time>();
	std::cout << "Day: " << timeReader.getDay() << std::endl;
}

void mysqlProcedure()
{
	MYSQL *connect;
	connect = mysql_init(NULL);

	if (!connect)
	{
		cout << "Mysql Initialization Failed";
		return 1;
	}

	connect = mysql_real_connect(connect, SERVER, USER, PASSWORD, DATABASE, 0, NULL, 0);

	if (connect)
	{
		cout << "Connection Succeeded\n";
	}
	else
	{
		cout << "Connection Failed\n";
	}

	MYSQL_RES *res_set;
	MYSQL_ROW row;

	// Replace MySQL query with your query

	mysql_query(connect, "show tables");

	unsigned int i = 0;

	res_set = mysql_store_result(connect);

	unsigned int numrows = mysql_num_rows(res_set);

	cout << " Tables in " << DATABASE << " database " << endl;

	while (((row = mysql_fetch_row(res_set)) != NULL))
	{
		cout << row[i] << endl;
	}

	mysql_query(connect, "insert into login values (1, 'Sealina', 'Keathalin21')");

	mysql_close(connect);

}

int main()
{
	// Assume a scenario where we want to query "select user from Users where region = ? and age = ?"
	Bind r_bind(std::vector<QueryParamSizeInfo>{ {TYPE_STRING, 20}, {TYPE_UI8, 0}});
	return 0;
}