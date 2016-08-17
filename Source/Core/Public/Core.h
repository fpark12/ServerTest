#pragma once


namespace CoreGlobals
{

};

class Core
{
	DECLARE_SINGLETON(Core)
public:
	Config& ConfigInstance = Singleton<Config>().Instance();
	Console& ConsoleInstance = Singleton<Console>().Instance();
	Logger& LoggerInstance = Singleton<Logger>().Instance();
	#define GConfig GCore.ConfigInstance
	#define GConsole GCore.ConsoleInstance
	#define GLog GCore.LoggerInstance

	//SslContext& GSslContextInst = Singleton<SslContext>().Instance();
	//#define GSslContext GCore.GSslContextInst


	int Init()
	{
		// Initialize Core components
		ConsoleInstance.Init();
		LoggerInstance.Init();

		return 0;
	}
};

static Core& GCoreInst = Singleton<Core>().Instance();
#define GCore GCoreInst

//static const Core& GCoreRef = Singleton<Core>().Instance();
//#define GCore GCoreRef
