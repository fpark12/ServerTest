
#include "class.h"

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


int main()
{
#define GConfig Config::instance()
	auto& configRef = Config::instance();
	bool result = GConfig.Load("config.ini");
	if (result)
	{
		string a = "";
		GConfig.GetString("section", "a", a, "config.ini");
		string b;
		GConfig.GetString("section", "a", b, "config.ini");
		auto c = configRef.GetKeys("config.ini");
	}
	std::mutex mu;
	SCOPED_LOCK(mu)
	{
		printf("idk what happened.\n");
	};

	{
		mu.lock();
		printf("idk what happened.\n");
		mu.unlock();
	}
	return 0;
}