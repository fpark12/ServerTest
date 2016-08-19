#pragma once

class SQLConnection;
class SQLOperation;

class SQLThread
{
	DISALLOW_COPY(SQLThread);

public:
	SQLThread();
	~SQLThread();

private:
	ProducerConsumerQueue<SQLOperation*>* SQLOperationTaskQueue;

	void WorkerThread();
	std::thread WorkingThread;

	std::atomic_bool CancelationToken;

};