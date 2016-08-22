#include "SQLThread.h"
#include "SQLConnectionPool.h"
#include "SQLOperation.h"

SQLThread::SQLThread() :
	CancelationToken(false),
	WorkingThread(&SQLThread::WorkerThread, this)
{
}

SQLThread::~SQLThread()
{
	CancelationToken = true;


	WorkingThread.join();
}

void SQLThread::WorkerThread()
{
	while (true)
	{

	}

	// check conditional variable
	/*
	while ( true )
	{
		sleep(10);
	}

	operation->Call();

	if (CancelationToken)
	{
		return;
	}
	//*/
}
