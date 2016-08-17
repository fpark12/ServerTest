#pragma once

class DatabaseConnection;
class SQLOperation;

class DatabaseWorker
{
public:
	DatabaseWorker(DatabaseConnection* connection);
	~DatabaseWorker();

	void SwitchConnection(DatabaseConnection* connection);

	bool IsFree()
	{
		return Free;
	}

private:
	std::mutex ResourceMutex;
	ProducerConsumerQueue<SQLOperation*>* SQLOperationTaskQueue;
	DatabaseConnection* MySQLConnectionHandle;

	void WorkerThread();
	std::thread WorkingThread;

	std::atomic_bool CancelationToken;
	std::atomic_bool Free;

	DatabaseWorker(DatabaseWorker const& right) = delete;
	DatabaseWorker& operator=(DatabaseWorker const& right) = delete;
};