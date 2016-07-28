#include "DatabaseOperation.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main()
{
	{
		MYSQL* mysql = mysql_init(NULL);

		MYSQL_STMT* stmt = mysql_stmt_init(mysql);
		if (!stmt)
		{
			exit(EXIT_FAILURE);
		}

		if (!mysql_real_connect(mysql, "localhost", "root", "Keathalin21", "testserver", 0, NULL, 0))
		{
			fprintf(stderr, "No connection could be made to the database\n");
			exit(EXIT_FAILURE);
		}

		char* sql = "SELECT `id`, `name`, `password` FROM `user` WHERE `sex` = ? AND `age` = ?";

		if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
		{
			exit(EXIT_FAILURE);
		}
		// error checking
		DatabaseOperation Op;
		Op.SetStatement(stmt);
		Op.SetParamInt32(0, 1);
		Op.SetParamInt32(1, 18);
		Op.ExecuteStatement();

		while (Op.GetNextRowOfResultSet())
		{
			int32 a = Op.GetInt32(0);
			std::string b = Op.GetString(1);
			std::string c = Op.GetString(2);
			std::cout << "Id: " << a << ", Name: " << b << ", Password: " << c << std::endl;
		}

		if (mysql_stmt_close(stmt))
		{
			fprintf(stderr, " failed while closing the statement\n");
			fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
			exit(EXIT_FAILURE);
		}
	}
	_CrtDumpMemoryLeaks();
}