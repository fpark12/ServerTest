#pragma once
#include "SQLOperation.h"
#include "Public/Detail/SQLDatabase.h"

class SQLTransaction : public SQLOperation
{
	DISABLE_COPY_AND_ASSIGN(SQLTransaction);

private:

	std::vector<std::string> Statements;

	std::mutex TransactionMutex;

public:

	// 1. Using the same transaction to operate 
	// more than once is not allowed
	// 2. stmt with binary params
	// are not allowed to be used with transaction
	// 3. select stmt or any stmt generate result
	// is not allowed to be used with transaction

	SQLTransaction()
	{

	}

	void AppendStatement(std::string _stmt_string)
	{
		Statements.push_back(_stmt_string);
	}

	// allows user to set parameter for the last stmt they appended
	uint32 SetParamInt(uint32 idx, int32 value)
	{
		if (Statements.empty())
		{
			return RC_FAILED;
		}

		std::string stmt = Statements.back();
		std::string::size_type searchCursor = -1;
		do 
		{
			searchCursor++;
			stmt.find('?', searchCursor);
		} while (idx-- && searchCursor != std::string::npos);

		if (searchCursor != std::string::npos)
		{
			stmt.replace(searchCursor, 1, std::to_string(value));
			return RC_SUCCESS;
		}
		else
		{
			return RC_FAILED;
		}
	}

	uint32 SetParamFloat(uint32 idx, double value)
	{
		if (Statements.empty())
		{
			return RC_FAILED;
		}

		std::string stmt = Statements.back();
		std::string::size_type searchCursor = -1;
		do
		{
			searchCursor++;
			stmt.find('?', searchCursor);
		} while (idx-- && searchCursor != std::string::npos);

		if (searchCursor != std::string::npos)
		{
			stmt.replace(searchCursor, 1, std::to_string(value));
			return RC_SUCCESS;
		}
		else
		{
			return RC_FAILED;
		}
	}

	uint32 SetParamString(uint32 idx, std::string value)
	{
		if (Statements.empty())
		{
			return RC_FAILED;
		}

		std::string stmt = Statements.back();
		std::string::size_type searchCursor = -1;
		do
		{
			searchCursor++;
			stmt.find('?', searchCursor);
		} while (idx-- && searchCursor != std::string::npos);

		if (searchCursor != std::string::npos)
		{
			stmt.replace(searchCursor, 1, value);
			return RC_SUCCESS;
		}
		else
		{
			return RC_FAILED;
		}
	}

	virtual uint32 Execute()
	{
		SQLConnection* conn = GDatabase.GetAvaliableSQLConnection(SQLOperation::SchemaIndex);
		MYSQL* mySql = SQLOperation::GetMySQLHandle(conn);

		// test validity of connection
		if (!conn)
		{
			GConsole.Message("{}: Connection to target schema unavailable.", __FUNCTION__);
			SQLOperation::OperationStatus = SQLOperation::SQLOperationStatus::Failed;
			return RC_FAILED;
		}

		// begin transaction
		if (Begin(mySql) != RC_SUCCESS)
		{
			return RC_FAILED;
		}

		for (auto& stmt: Statements)
		{
			if (mysql_query(mySql, stmt.c_str()))
			{
				const char* err = mysql_error(mySql);
				GConsole.Message("{}: Error executing \"START TRANSACTION\": {}.", __FUNCTION__, err);
				SQLOperation::OperationStatus = SQLOperation::SQLOperationStatus::Failed;
				if (RollBack(mySql) != RC_SUCCESS)
				{
					GConsole.Message("{}: Rollback for last transaction unsuccessful, database might be tinted.", __FUNCTION__);
				}
				return RC_FAILED;
			}

			return RC_SUCCESS;
		}


		return RC_SUCCESS;
	}

private:

	uint32 Begin(MYSQL* mySqlHandle);

	uint32 Commit(MYSQL* mySqlHandle);

	uint32 RollBack(MYSQL* mySqlHandle);

};