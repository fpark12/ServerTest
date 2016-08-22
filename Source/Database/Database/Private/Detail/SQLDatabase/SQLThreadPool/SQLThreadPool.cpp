#include "Private/Detail/SQLDatabase/SQLThreadPool/SQLThreadPool.h"


void SQLThreadPool::SpawnThread()
{
	if (Threads.size() < MaxThreadCount)
	{
		Threads.push_back(new SQLThread());
	}
}
