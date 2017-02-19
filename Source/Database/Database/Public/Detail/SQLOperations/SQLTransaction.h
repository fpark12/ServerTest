#pragma once
#include "Private/Detail/SQLOperations/SQLOperationBase/SQLOperationBase.h"
#include "Public/Detail/SQLDatabase.h"

class SQLTransaction : public SQLOperationBase
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
	Status SetParamInt(uint32 idx, int32 value)
	{
		if (Statements.empty())
		{
			return Status::FAILED;
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
			return Status::OK;
		}
		else
		{
			return Status::FAILED;
		}
	}

	Status SetParamFloat(uint32 idx, double value)
	{
		if (Statements.empty())
		{
			return Status::FAILED;
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
			return Status::OK;
		}
		else
		{
			return Status::FAILED;
		}
	}

	Status SetParamString(uint32 idx, std::string value)
	{
		if (Statements.empty())
		{
			return Status::FAILED;
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
			return Status::OK;
		}
		else
		{
			return Status::FAILED;
		}
	}

	virtual Status Execute()
	{
		SQLConnection* conn = GDatabase.GetAvaliableSQLConnection(SQLOperationBase::SchemaIndex);
		MYSQL* mySql = SQLOperationBase::GetMySQLHandle(conn);

		// test validity of connection
		if (!conn)
		{
			GConsole.Message("{}: Connection to target schema unavailable.", __FUNCTION__);
			SQLOperationBase::OperationStatus = SQLOperationBase::SQLOperationStatus::Failed;
			return Status::FAILED;
		}

		// begin transaction
		if (Begin(mySql) != Status::OK)
		{
			return Status::FAILED;
		}

		for (auto& stmt: Statements)
		{
			if (mysql_query(mySql, stmt.c_str()))
			{
				const char* err = mysql_error(mySql);
				GConsole.Message("{}: Error executing \"START TRANSACTION\": {}.", __FUNCTION__, err);
				SQLOperationBase::OperationStatus = SQLOperationBase::SQLOperationStatus::Failed;
				if (RollBack(mySql) != Status::OK)
				{
					GConsole.Message("{}: Rollback for last transaction unsuccessful, database might be tinted.", __FUNCTION__);
				}
				return Status::FAILED;
			}

			return Status::OK;
		}


		return Status::OK;
	}

private:

	Status Begin(MYSQL* mySqlHandle);

	Status Commit(MYSQL* mySqlHandle);

	Status RollBack(MYSQL* mySqlHandle);

};