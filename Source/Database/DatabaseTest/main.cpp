#include "Public/Detail/SQLOperation.h"
#include "SQLConnection.h"
#include "DatabaseWorker.h"

#if PLATFORM_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

std::mutex queryMutex;

/*
void QueryAndPrint(MYSQL* mysql)
{
	MYSQL_STMT* stmt = mysql_stmt_init(mysql);
	if (!stmt)
	{
		exit(EXIT_FAILURE);
	}

	char* sql = "SELECT `sex`, `age`, `score` FROM `user` WHERE `name` = ?";

	queryMutex.lock();
	if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
	{
		exit(EXIT_FAILURE);
	}
	queryMutex.unlock();

	DatabaseOperation Op;
	Op.SetStatement(stmt);
	Op.SetParamString(0, "Sealina");
	Op.Execute();

	while (Op.GetNextRowOfResultSet())
	{
		int32 a = Op.GetInt32(0);
		int32 b = Op.GetInt32(1);
		float c = Op.GetFloat(2);
		std::cout << "Sex: " << a << ", Age: " << b << ", Score: " << c << std::endl;
	}

	if (mysql_stmt_close(stmt))
	{
		fprintf(stderr, " failed while closing the statement\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}
}

void QueryAndPrint2(MYSQL* mysql)
{
	MYSQL_STMT* stmt = mysql_stmt_init(mysql);
	if (!stmt)
	{
		exit(EXIT_FAILURE);
	}

	char* sql = "SELECT `sex`, `age`, `name` FROM `user` WHERE `id` = ?";

	queryMutex.lock();
	if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
	{
		exit(EXIT_FAILURE);
	}
	queryMutex.unlock();

	DatabaseOperation Op;
	Op.SetStatement(stmt);
	Op.SetParamInt32(0, 3);
	Op.Execute();

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
*/
BEFORE_MAIN()
{
	GCore.Init();
}

int main()
{
	GConsole.Print("abcd");

	{
		/*MYSQL* mysql = mysql_init(NULL);

		if (!mysql_real_connect(mysql, "localhost", "root", "Keathalin21", "testserver", 0, NULL, 0))
		{
			fprintf(stderr, "No connection could be made to the database\n");
			exit(EXIT_FAILURE);
		}
		
		std::thread t1(QueryAndPrint, mysql);

		std::thread t2(QueryAndPrint2, mysql);*/

		/*MYSQL* connections[4];

		for (int i = 0; i < 4; i++)
		{
			MYSQL *m;

			m = mysql_init(NULL);
			m->free_me = 1;

			if (!mysql_real_connect(m, "127.0.0.1", "root", "Keathalin21", NULL, 3306, NULL, 0))
			{
				printf("%s (%d)", mysql_error(m), mysql_errno(m));
				exit(EXIT_FAILURE);
			}

			connections[i] = m;
		}*/

		/*
		MYSQL* mysql = mysql_init(NULL);

		if (!mysql_real_connect(mysql, "localhost", "root", "Keathalin21", "testserver", 0, NULL, 0))
		{
			fprintf(stderr, "No connection could be made to the database\n");
			exit(EXIT_FAILURE);
		}


		
		std::thread t1([]() {
			MYSQL* mysql = mysql_init(NULL);

			if (!mysql_real_connect(mysql, "localhost", "root", "Keathalin21", "testserver", 3306, NULL, 0))
			{
				fprintf(stderr, "t1: No connection could be made to the database\n");
				exit(EXIT_FAILURE);
			}

			MYSQL_STMT* stmt = mysql_stmt_init(mysql);
			if (!stmt)
			{
				exit(EXIT_FAILURE);
			}

			char* sql = "SELECT `sex`, `age`, `name` FROM `user` WHERE `id` = ?";

			if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
			{
				exit(EXIT_FAILURE);
			}

			SQLOperation Op;
			Op.SetStatement(stmt);
			Op.SetParamInt32(0, 1);
			Op.Execute();

			while (Op.GetNextRowOfResultSet())
			{
				int32 a = Op.GetInt32(0);
				int32 b = Op.GetInt32(1);
				std::string c = Op.GetString(2);
				std::lock_guard<std::mutex> lock1(queryMutex);
				std::cout << "t1: Sex: " << a << ", Age: " << b << ", Name: " << c << std::endl;
			}

			if (mysql_stmt_close(stmt))
			{
				fprintf(stderr, " failed while closing the statement\n");
				fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
				exit(EXIT_FAILURE);
			}
		});

		std::thread t2([]()
		{
			MYSQL* mysql = mysql_init(NULL);

			if (!mysql_real_connect(mysql, "localhost", "root", "Keathalin21", "testserver", 3306, NULL, 0))
			{
				fprintf(stderr, "t2: No connection could be made to the database\n");
				exit(EXIT_FAILURE);
			}


			MYSQL_STMT* stmt = mysql_stmt_init(mysql);
			if (!stmt)
			{
				exit(EXIT_FAILURE);
			}

			char* sql = "SELECT `sex`, `age`, `name` FROM `user` WHERE `id` = ?";

			if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
			{
				exit(EXIT_FAILURE);
			}

			SQLOperation Op;
			Op.SetStatement(stmt);
			Op.SetParamInt32(0, 1);
			Op.Execute();

			while (Op.GetNextRowOfResultSet())
			{
				int32 a = Op.GetInt32(0);
				int32 b = Op.GetInt32(1);
				std::string c = Op.GetString(2);
				std::lock_guard<std::mutex> lock1(queryMutex);
				std::cout << "t2: Sex: " << a << ", Age: " << b << ", Name: " << c << std::endl;
			}

			if (mysql_stmt_close(stmt))
			{
				fprintf(stderr, " failed while closing the statement\n");
				fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
				exit(EXIT_FAILURE);
			}

		});


		t1.join();
		t2.join();

		if (t1.joinable())
		{
			t1.join();
		}
		if (t2.joinable())
		{
			t2.join();
		}
*/

		//std::string s = fmt::format("{0}{1}{0}", "abra", "cad");

		GConfig.Load("Database.ini");
		bool Res;
		DatabaseConnectionInfo dbInfo;
		Res |= GConfig.GetString("Database.Initialization.Hostname", dbInfo.Hostname);
		Res |= GConfig.GetString("Database.Initialization.Username", dbInfo.Username);
		Res |= GConfig.GetString("Database.Initialization.Password", dbInfo.Password);
		Res |= GConfig.GetString("Database.Initialization.Schema", dbInfo.Schema);
		Res |= GConfig.GetUInt("Database.Initialization.Port", dbInfo.Port);

		SQLConnection dbConn(dbInfo);
		char* sql = "SELECT `sex`, `age`, `name` FROM `user` WHERE `id` = ?";

		SQLOperation op(&dbConn);
		std::string test = "abc";// "tempString1tempString2tempString3tempString4tempString5tempString6";
		op.SetStatement(sql);
		op.MoveParamString(0, std::move(test));
		op.SetParamInt32(0, 1);
		memset(&test, 0, sizeof(std::string));

		dbConn.AddTask(&op);
		DatabaseWorker worker(&dbConn);

		while (!op.IsDone());

		while (op.GetNextRowOfResultSet())
		{
			int32 a = op.GetResultInt32(0);
			int32 b = op.GetResultInt32(1);
			std::string c = op.GetResultString(2);
			std::cout << "Sex: " << a << ", Age: " << b << ", Name: " << c << std::endl;
		}
	}
#if PLATFORM_WINDOWS
	_CrtDumpMemoryLeaks();
#endif
}