#pragma once

class SQLConnection;
class SQLOperation;

class DatabaseWorker
{
public:
	DatabaseWorker(SQLConnection* connection);
	~DatabaseWorker();

	void SwitchConnection(SQLConnection* connection);

	bool IsFree()
	{
		return Free;
	}

private:
	std::mutex ResourceMutex;
	ProducerConsumerQueue<SQLOperation*>* SQLOperationTaskQueue;
	SQLConnection* MySQLConnectionHandle;

	void WorkerThread();
	std::thread WorkingThread;

	std::atomic_bool CancelationToken;
	std::atomic_bool Free;

	DatabaseWorker(DatabaseWorker const& right) = delete;
	DatabaseWorker& operator=(DatabaseWorker const& right) = delete;
};