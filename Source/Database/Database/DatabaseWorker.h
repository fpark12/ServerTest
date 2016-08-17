#pragma once

class DatabaseConnection;
class SQLOperation;

class DatabaseWorker
{
public:
	DatabaseWorker(std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> newQueue, std::unique_ptr<DatabaseConnection> connection);
	~DatabaseWorker();

	bool SwitchConnection(std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> newQueue, std::unique_ptr<DatabaseConnection> connection);

	bool IsFree()
	{
		return Free;
	}

private:
	std::mutex ResourceMutex;
	std::unique_ptr<ProducerConsumerQueue<SQLOperation*>> SQLOperationTaskQueue;
	std::unique_ptr<DatabaseConnection> MySQLConnectionHandle;

	void WorkerThread();
	std::thread WorkingThread;

	std::atomic_bool CancelationToken;
	std::atomic_bool Free;

	DatabaseWorker(DatabaseWorker const& right) = delete;
	DatabaseWorker& operator=(DatabaseWorker const& right) = delete;
};