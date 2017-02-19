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

Status SQLTask::Execute()
{
	return Operation->Execute();
}
