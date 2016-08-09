#include "DatabaseOperation.h"
#include <stdlib.h>

#if PLATFORM_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

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

		char sql [] = "SELECT `sex`, `age`, `name` FROM `user` WHERE `score` = ?";

		if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
		{
			exit(EXIT_FAILURE);
		}
		// error checking
		DatabaseOperation Op;
		Op.SetStatement(stmt);
		Op.SetParamFloat(0, 3.4);
		Op.ExecuteStatement();

		while (Op.GetNextRowOfResultSet())
		{
			int32 a = Op.GetInt32(0);
			int32 b = Op.GetInt32(1);
			std::string c = Op.GetString(2);
			std::cout << "Sex: " << a << ", Age: " << b << ", Name: " << c << std::endl;
		}

		if (mysql_stmt_close(stmt))
		{
			fprintf(stderr, " failed while closing the statement\n");
			fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
			exit(EXIT_FAILURE);
		}
	}
#if PLATFORM_WINDOWS
	_CrtDumpMemoryLeaks();
#endif
}