#include "Public/Detail/SQLOperation.h"
#include "SQLConnection.h"
#include "SQLThread.h"

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
	GConsole.Print("abcd");

	{

		GConfig.Load("Database.ini");
		bool Res;
		SQLConnectionInfo dbInfo;
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