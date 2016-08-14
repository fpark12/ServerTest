#pragma once

class Core
{
	template<class U> friend class Singleton;
private:
	Core() = default;
	~Core() = default;
	Core(Core const&) = delete;
	Core& operator=(Core const&) = delete;

public:

	static Core& Instance()
	{
		static Core Instance;
		return Instance;
	}
};

#define GCore GCoreRef
static Core& GCoreRef = Core::Instance();