#pragma once

class SQLConnection;
class SQLOperation;

class SQLThread
{
	DISABLE_COPY(SQLThread);

public:
	SQLThread();
	~SQLThread();

private:
	void WorkerThread();
	std::thread WorkingThread;

	std::atomic_bool CancelationToken;

};