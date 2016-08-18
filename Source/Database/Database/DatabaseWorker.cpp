#include "DatabaseWorker.h"
#include "SQLOperation.h"
#include "SQLConnection.h"

DatabaseWorker::DatabaseWorker(SQLConnection* connection)
{
	SCOPED_LOCK(ResourceMutex)
	{
		MySQLConnectionHandle = connection;
		SQLOperationTaskQueue = connection->OperationQueue;
	};
	CancelationToken = false;
	Free = false;
	WorkingThread = std::thread(&DatabaseWorker::WorkerThread, this);
}

DatabaseWorker::~DatabaseWorker()
{
	CancelationToken = true;

	SQLOperationTaskQueue->Cancel();

	WorkingThread.join();
}

void DatabaseWorker::SwitchConnection(SQLConnection* connection)
{
	SCOPED_LOCK(ResourceMutex)
	{
		MySQLConnectionHandle = connection;
		SQLOperationTaskQueue = connection->OperationQueue;
	};
}

void DatabaseWorker::WorkerThread()
{
	while (!SQLOperationTaskQueue)
	{
		if (CancelationToken)
		{
			return;
		}
		Free = true;
	}
	Free = false;

	for (;;)
	{
		SQLOperation* operation = nullptr;

		SCOPED_LOCK(ResourceMutex)
		{
			SQLOperationTaskQueue->WaitAndPop(operation);
		};

		while (!operation)
		{
			Free = true;
			if (CancelationToken)
			{
				return;
			}

			{
				std::lock_guard<std::mutex> lock(ResourceMutex);
				SQLOperationTaskQueue->WaitAndPop(operation);
			}
		}
		Free = false;

		operation->SetConnection(MySQLConnectionHandle);
		operation->Call();

		if (CancelationToken)
		{
			return;
		}
	}
}
