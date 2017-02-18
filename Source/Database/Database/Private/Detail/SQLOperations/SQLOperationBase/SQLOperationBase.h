#pragma once
#include "Private\Detail\SQLDatabase\SQLConnectionPool\SQLConnection.h"

// base class for single statement and multi statements operation
class SQLOperationBase
{
	DISABLE_COPY_AND_ASSIGN(SQLOperationBase);

// accessible by child class
protected:

	// tracking running status
	enum class SQLOperationStatus
	{
		Success,
		Failed,
		Running
	};

	SQLOperationStatus OperationStatus;

	// Schema related
	uint16 SchemaIndex;
	// auto assigned sql connection resources when executing

public:

	// Virtual Execute Function
	virtual uint32 Execute() = 0;

	virtual void AsyncExecute();

protected:

	SQLOperationBase();

	// also allows children to access mysql handle in connection, 
	// but protect connection private data from general public
	static MYSQL* GetMySQLHandle(SQLConnection* _conn);

	static void FreeUpConnection(SQLConnection* _conn);

};
