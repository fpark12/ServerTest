#include "SQLOperation.h"
#include "Public/Detail/SQLDatabase.h"

void SQLOperation::AsyncExecute()
{
	// TODO: whether heap is needed to create async tasks and operations
	GDatabase.AddTask(new SQLTask(this));
}

SQLOperation::SQLOperation() : OperationStatus(SQLOperationStatus::Running)
{

}

MYSQL* SQLOperation::GetMySQLHandle(SQLConnection* _conn)
{
	return _conn->MySqlHandle;
}

void SQLOperation::FreeUpConnection(SQLConnection* _conn)
{
	bool Expected = false;
	_conn->IsAvaliable.compare_exchange_strong(Expected, true);
}
