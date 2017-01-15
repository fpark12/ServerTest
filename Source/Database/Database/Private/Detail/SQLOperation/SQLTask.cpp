#include "SQLTask.h"
#include "SQLOperation.h"

SQLTask::SQLTask()
{

}

SQLTask::SQLTask(SQLOperation* _operation) : Operation(_operation)
{

}

SQLTask::~SQLTask()
{

}

uint32 SQLTask::Execute()
{
	return Operation->Execute();
}
