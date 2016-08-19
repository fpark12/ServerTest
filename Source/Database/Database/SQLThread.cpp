#include "SQLThread.h"
#include "SQLConnectionPool.h"
#include "SQLOperation.h"

SQLThread::SQLThread() :
	SQLOperationTaskQueue(&ConnectionPool.TaskQueue),
	CancelationToken(false),
	WorkingThread(&SQLThread::WorkerThread, this)
{
}

SQLThread::~SQLThread()
{
	CancelationToken = true;

	SQLOperationTaskQueue->Cancel();

	WorkingThread.join();
}

void SQLThread::WorkerThread()
{
	while (!SQLOperationTaskQueue)
	{
		if (CancelationToken)
		{
			return;
		}
	}

	while (true)
	{
		SQLOperation* operation = nullptr;

		SQLOperationTaskQueue->WaitAndPop(operation);

		while (!operation)
		{
			if (CancelationToken)
			{
				return;
			}
		}

		operation->Call();

		if (CancelationToken)
		{
			return;
		}
	}
}
