#include "Public/Detail/SQLDatabase.h"
#include "Public/Detail/SQLThreadPool.h"
#include "Public/Detail/SQLOperation.h"

#if PLATFORM_WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


BEFORE_MAIN()
{
	GCore.Init();
}

int main()
{
	//GConsole.Print("abcd");

	{
		GConfig.Load("DatabaseTest.ini");
		bool Res;
		SQLSchemaInfo SchemaInfo;
		Res |= GConfig.GetString("DatabaseTest.LoginDatabase.Hostname", SchemaInfo.Hostname);
		Res |= GConfig.GetString("DatabaseTest.LoginDatabase.Username", SchemaInfo.Username);
		Res |= GConfig.GetString("DatabaseTest.LoginDatabase.Password", SchemaInfo.Password);
		Res |= GConfig.GetString("DatabaseTest.LoginDatabase.Schema", SchemaInfo.Schema);
		Res |= GConfig.GetUInt("DatabaseTest.LoginDatabase.Port", SchemaInfo.Port);
		Res |= GConfig.GetUInt("DatabaseTest.LoginDatabase.ConnectionCount", SchemaInfo.ConnectionCount);
		Res |= GConfig.GetUInt("DatabaseTest.LoginDatabase.ThreadingMode", SchemaInfo.ThreadingMode);

		enum ESchema
		{
			Login = 0
		};

		enum ELoginPreparedStatement
		{
			UsernameQuery = 0,
			PersonalInfoQuery = 1
		};

		GDatabase.AddSchema(ESchema::Login, SchemaInfo);
		ASSERT(GDatabase.SpawnSQLConnections() == RC_SUCCESS);

		SQLOperation operation(ESchema::Login, ELoginPreparedStatement::UsernameQuery);
		operation(ESchema::Login, ELoginPreparedStatement::UsernameQuery);
		operation.SetStatement("DROP TABLE IF EXISTS debug");
		operation.Execute();
		operation.SetStatement("CREATE TABLE debug (id int not null, my_name varchar(50), PRIMARY KEY(id))");
		operation.Execute();

		operation.SetStatement("DROP TABLE IF EXISTS debug");

		for (int i = 0; i < 10; i++)
		{
			GDatabase.AddTask(&operation);
		}

		GSQLThreadPool.SetDefaultThreadCount();
		GSQLThreadPool.SpawnThreads();

		GConsole.Message("{}: Time: {}", __FUNCTION__, getMSTime());
		while (!operation.Completed())
		{
			int i = 0;
		}
		GConsole.Message("{}: Time: {}", __FUNCTION__, getMSTime());
		//operation.SetStatement("SELECT `sex`, `age`, `name` FROM `user` WHERE `id` = ?");
		//operation.SetParamInt32(0, 1);
		for (int i = 0; i < 1000; i++)
		{
			operation.Call();
		}




		/*
		Database.Schemas[enum::LoginDatabase];
		SQLOperation op(LoginConnectionPool);
		op.SetPreparedStatement(enum::GET_CHARACTERS);
		op.SetParam(0, xxx);
		GDatabase.AsyncQuery(op);
		//Database.Connect();
		SQLConnection dbConn(dbInfo);
		char* sql = "SELECT `sex`, `age`, `name` FROM `user` WHERE `id` = ?";

		SQLOperation op(&dbConn);
		std::string test = "abc";// "tempString1tempString2tempString3tempString4tempString5tempString6";
		op.SetStatement(sql);
		op.MoveParamString(0, std::move(test));
		op.SetParamInt32(0, 1);
		memset(&test, 0, sizeof(std::string));

		//dbConn.AddTask(&op);
		//SQLThread worker(&dbConn);

		while (!op.IsDone());

		while (op.GetNextRowOfResultSet())
		{
			int32 a = op.GetResultInt32(0);
			int32 b = op.GetResultInt32(1);
			std::string c = op.GetResultString(2);
			std::cout << "Sex: " << a << ", Age: " << b << ", Name: " << c << std::endl;
		}
		*/

	}
#if PLATFORM_WINDOWS
	_CrtDumpMemoryLeaks();
#endif
}