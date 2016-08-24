#include "SQLThread.h"
#include "SQLConnectionPool.h"
#include "SQLOperation.h"
#include "SQLDatabase.h"

SQLThread::SQLThread() :
	CancelationToken(false),
	WorkingThread(&SQLThread::WorkerThread, this)
{
}

SQLThread::~SQLThread()
{
	CancelationToken = true;
	WorkingThread.join();
}

void SQLThread::WorkerThread()
{
	while (!CancelationToken)
	{
		static SQLOperation* nextTask = nullptr;
		if (nextTask = GDatabase.NextTask())
		{
			nextTask->Call();
		}
	}
}
