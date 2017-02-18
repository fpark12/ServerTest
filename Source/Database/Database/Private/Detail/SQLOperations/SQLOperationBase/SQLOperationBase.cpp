#include "Private/Detail/SQLOperations/SQLOperationBase/SQLOperationBase.h"
#include "Public/Detail/SQLDatabase.h"

void SQLOperationBase::AsyncExecute()
{
	// TODO: whether heap is needed to create async tasks and operations
	GDatabase.AddTask(new SQLTask(this));
}

SQLOperationBase::SQLOperationBase() : OperationStatus(SQLOperationStatus::Running)
{

}

MYSQL* SQLOperationBase::GetMySQLHandle(SQLConnection* _conn)
{
	return _conn->MySqlHandle;
}

void SQLOperationBase::FreeUpConnection(SQLConnection* _conn)
{
	bool Expected = false;
	_conn->IsAvaliable.compare_exchange_strong(Expected, true);
}
