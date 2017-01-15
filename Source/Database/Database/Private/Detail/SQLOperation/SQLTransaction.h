#pragma once
#include "SQLOperation.h"

class SQLTransaction : public SQLOperation
{
	DISABLE_COPY_AND_ASSIGN(SQLTransaction);

public:
	void AppendOperation(SQLOperation&&);

	void CommitOperation();

	void RollBack();

	virtual uint32 Execute()
	{
		for (auto& op: Operations)
		{
			op.Execute();
		}
	}

private:
	std::vector<SQLOperation> Operations;

	std::mutex OperationsMutex;

};