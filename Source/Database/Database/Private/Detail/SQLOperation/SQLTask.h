#pragma once

class SQLTask
{
	DISABLE_COPY_AND_ASSIGN(SQLTask);

	// only SQLOperation can create task
	friend class SQLOperation;

private:

	SQLOperation* Operation;

public:

	~SQLTask();

	uint32 Execute();

protected:
	// disable SQLTask creation, 
	// it will be created when Operation gets executed
	SQLTask();

	SQLTask(SQLOperation* _operation);
};