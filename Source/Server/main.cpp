#include "Server.h"

BEFORE_MAIN()
{
	GCore.Init();
}
int main()
{
	asio::io_service IoService;
	//GConsole.Print("w/e man.");
	//GLog.Critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);

	bool result = GConfig.Load("config.ini");
	result = GConfig.Load("LoginService.ini");
	GLoginService.Start(IoService);
	if (result)
	{
		auto names = GConfig.GetFilenames();
		std::string a = "";
		GConfig.GetString("section", "teststring", a, "config.ini");
		//GConfigRef2.GetString("section", "teststring", a, "config.ini");
		std::string b;
		GConfig.GetString("section", "teststring", b, "config.ini");
		auto c = GConfig.GetKeys("config.ini");
	}


	std::mutex mu;
	SCOPED_LOCK(mu)
	{
		printf("idk what happened.\n");
	};

	{
		std::lock_guard<std::mutex> lock(mu);
		printf("idk what happened.\n");
	}



	return 0;
}