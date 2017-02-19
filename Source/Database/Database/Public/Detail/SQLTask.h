#pragma once

class SQLTask
{
	DISABLE_COPY_AND_ASSIGN(SQLTask);

	// only SQLOperation can create task
	friend class SQLOperationBase;

private:

	SQLOperationBase* Operation;

public:

	~SQLTask();

	Status Execute();

protected:
	// disable SQLTask creation, 
	// it will be created when Operation gets executed
	SQLTask();

	SQLTask(SQLOperationBase* _operation);
};