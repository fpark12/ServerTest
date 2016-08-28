#include "SQLThread.h"
#include "SQLOperation.h"
#include "SQLDatabase.h"

SQLThread::SQLThread() :
	CancelationToken(false)
{
	WorkingThread = std::thread(&SQLThread::Main, this);
}

SQLThread::SQLThread(SQLThread&& Other) :
	CancelationToken(Other.CancelationToken.load())
{
	WorkingThread = std::thread(std::move(Other.WorkingThread));
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
