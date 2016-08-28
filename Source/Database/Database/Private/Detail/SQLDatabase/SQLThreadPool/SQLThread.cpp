#include "SQLThread.h"
#include "SQLOperation.h"
#include "SQLDatabase.h"

SQLThread::SQLThread() :
	CancelationToken(false),
	WorkingThread(&SQLThread::Main, this)
{
}

SQLThread::~SQLThread()
{
	CancelationToken = true;
	WorkingThread.join();
}

void SQLThread::Main()
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
