#include "SQLThread.h"
#include "Private/Detail/SQLOperations/SQLOperationBase/SQLOperationBase.h"
#include "Public/Detail/SQLDatabase.h"

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
		static SQLTask* nextTask = nullptr;
		//TODO sleep if no new task, followed by a signal from task queue if new job comes
		if (nextTask = GDatabase.NextTask())
		{
			nextTask->Execute();
		}
	}
}
