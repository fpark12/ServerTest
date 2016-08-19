#pragma once
#include "Private/Detail/SQLDatabase/SQLConnectionPool/SQLConnectionPool.h"
#include "Private/Detail/SQLDatabase/SQLThreadPool/SQLThreadPool.h"

class SQLDatabase
{
private:
	SQLConnectionPool ConnectionPool;
	SQLThreadPool ThreadPool;

};