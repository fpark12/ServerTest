#include "Singleton.h"

Singleton& Singleton::Instance()
{
	static Singleton Instance;
	return Instance;
}
