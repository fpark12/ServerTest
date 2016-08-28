#pragma once

class SQLConnection;
class SQLOperation;

class SQLThread
{
	DISABLE_COPY_AND_ASSIGN(SQLThread);
	friend class SQLThreadPool;
public:
	SQLThread();

	SQLThread(std::function<void()> Function) :
		WorkingThread(Function)
	{

	}
	~SQLThread();

private:
	void Main();
	std::thread WorkingThread;

	std::atomic_bool CancelationToken;

};