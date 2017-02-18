#include "Public/Detail/SQLOperations/SQLTransaction.h"

uint32 SQLTransaction::Begin(MYSQL* mySqlHandle)
{
	if (mysql_query(mySqlHandle, "START TRANSACTION"))
	{
		const char* err = mysql_error(mySqlHandle);
		GConsole.Message("{}: Error executing \"START TRANSACTION\": {}.", __FUNCTION__, err);
		SQLOperationBase::OperationStatus = SQLOperationBase::SQLOperationStatus::Failed;
		return RC_FAILED;
	}

	return RC_SUCCESS;
}

uint32 SQLTransaction::Commit(MYSQL* mySqlHandle)
{
	if (mysql_query(mySqlHandle, "COMMIT"))
	{
		const char* err = mysql_error(mySqlHandle);
		GConsole.Message("{}: Error executing \"COMMIT\": {}.", __FUNCTION__, err);
		SQLOperationBase::OperationStatus = SQLOperationBase::SQLOperationStatus::Failed;
		return RC_FAILED;
	}

	return RC_SUCCESS;
}

uint32 SQLTransaction::RollBack(MYSQL* mySqlHandle)
{
	if (mysql_query(mySqlHandle, "ROLLBACK"))
	{
		const char* err = mysql_error(mySqlHandle);
		GConsole.Message("{}: Error executing \"ROLLBACK\": {}.", __FUNCTION__, err);
		SQLOperationBase::OperationStatus = SQLOperationBase::SQLOperationStatus::Failed;
		return RC_FAILED;
	}

	return RC_SUCCESS;
}
