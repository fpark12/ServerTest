#pragma once

class SQLConnection;
class SQLOperationBase;

class SQLThread
{
	DISABLE_COPY_AND_ASSIGN(SQLThread);
	friend class SQLThreadPool;
public:
	SQLThread();
	SQLThread(SQLThread&& Other);

	SQLThread(std::function<void()> Function) :
		WorkingThread(Function)
	{

	}
	~SQLThread();

private:
	void Main();
	std::thread WorkingThread;

	std::atomic<bool> CancelationToken;

};