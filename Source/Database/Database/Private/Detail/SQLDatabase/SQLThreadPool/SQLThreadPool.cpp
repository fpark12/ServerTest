#include "Public/Detail/SQLThreadPool.h"


void SQLThreadPool::SpawnThreads()
{
	SharedThreads.resize(SharedThreadCount);
	for(int i = 0; i < SharedThreadCount; i++)
	{
		SharedThreads[i] = std::move(std::unique_ptr<SQLThread>(new SQLThread(&SQLThread::Main)));
	}
}
