#pragma once
#include "Public/Detail/Config.h"
#include "Public/Detail/Logger.h"

class Core
{
	DECLARE_SINGLETON(Core)
public:

};

namespace CoreGlobals
{
	#define GCore CoreGlobals::GCoreInst
	#define GConfig CoreGlobals::GConfigRef
	static Core& GCoreInst = Singleton<Core>().Instance();
	static Config& GConfigRef = Singleton<Config>().Instance();
};

//static const Core& GCoreRef = Singleton<Core>().Instance();
//#define GCore GCoreRef
