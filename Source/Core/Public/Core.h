#pragma once


namespace CoreGlobals
{

};

class Core
{
	DECLARE_SINGLETON(Core)
public:
	Config& ConfigInstance = Singleton<Config>().Instance();
	Logger& LoggerInstance = Singleton<Logger>().Instance();
	#define GConfig GCore.ConfigInstance
	#define GLog GCore.LoggerInstance

	int Init()
	{
		// Initialize Core components
		LoggerInstance.Init();

		return 0;
	}
};

static Core& GCoreInst = Singleton<Core>().Instance();
#define GCore GCoreInst

//static const Core& GCoreRef = Singleton<Core>().Instance();
//#define GCore GCoreRef
