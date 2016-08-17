#include "DatabaseWorker.h"
#include "SQLOperation.h"
#include "DatabaseConnection.h"

DatabaseWorker::DatabaseWorker(std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> newQueue, std::unique_ptr<DatabaseConnection> connection)
{
	std::lock_guard<std::mutex> lock(ResourceMutex);
	MySQLConnectionHandle = std::move(connection);
	SQLOperationTaskQueue = std::move(newQueue);
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

bool DatabaseWorker::SwitchConnection(std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> newQueue, std::unique_ptr<DatabaseConnection> connection)
{
	std::lock_guard<std::mutex> lock(ResourceMutex);
	MySQLConnectionHandle = std::move(connection);
	SQLOperationTaskQueue = std::move(newQueue);
	return true;
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

		delete operation;

		if (CancelationToken)
		{
			return;
		}
	}
}
