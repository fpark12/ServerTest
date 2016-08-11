
#include "class.h"
#include "asio.hpp"
#include "LoginService.h"

using namespace std;

class LogFile
{
	std::mutex _mu;
	ofstream _f;

public:
	LogFile()
	{
		_f.open("log.txt");
	}

	~LogFile()
	{

	}
	void shared_print(std::string msg, int id)
	{
		std::lock_guard<mutex> locker(_mu);
		_f << "From " << id << ": " << msg << endl;
	}

private:

};


::LoginService& LoginServiceRef = LoginService::Instance();

Ssl& SslRef = Ssl::Instance();


int main()
{
	asio::io_service IoService;
	//Config& GConfigRef2 = Config::Instance();

	Config::Instance();
	bool result = GConfig.Load("config.ini");
	result = GConfig.Load("LoginService.ini");
	GLoginService.Start(IoService);
	if (result)
	{
		auto names = GConfig.GetFilenames();
		string a = "";
		GConfig.GetString("section", "teststring", a, "config.ini");
		//GConfigRef2.GetString("section", "teststring", a, "config.ini");
		string b;
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