#include "Public/Detail/SQLTask.h"
#include "Private/Detail/SQLOperations/SQLOperationBase/SQLOperationBase.h"

SQLTask::SQLTask()
{

}

SQLTask::SQLTask(SQLOperationBase* _operation) : Operation(_operation)
{

}

SQLTask::~SQLTask()
{

}

uint32 SQLTask::Execute()
{
	return Operation->Execute();
}
