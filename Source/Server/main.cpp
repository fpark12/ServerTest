#include "LoginService.h"

BEFORE_MAIN()
{
	GCore;
	GLog.Init();
}
int main()
{
	asio::io_service IoService;
	//Config& GConfigRef2 = Config::Instance();


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