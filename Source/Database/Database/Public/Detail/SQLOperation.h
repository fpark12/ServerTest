#pragma once
#include "Private/Detail/SQLOperation/SQLOperationParamsArchive.h"
#include "Private/Detail/SQLOperation/SQLOperationResultSet.h"

class SQLConnection;


enum class SQLOperationFlag
{
	Prepared,
	RawString,
	RawStringPrepared,
	Neither
};

enum class SQLOperationStatus
{
	Success,
	Failed,
	None,
	Count
};


class SQLOperation : public SQLOperationParamsArchive, public SQLOperationResultSet
{
	DISABLE_COPY_AND_ASSIGN(SQLOperation);

private:

	// General
	union
	{
		uint16 SchemaIndex;
		SQLConnection* Connection;
	};

	union
	{
		uint16 PreparedStatementIndex;
		MYSQL_STMT* MYSQLStatement;
		char* RawStringStatement;
	};

	SQLOperationFlag OperationFlag;
	std::atomic<SQLOperationStatus> OperationStatus = SQLOperationStatus::None;

public:

	SQLOperation(SQLConnection* conn);
	SQLOperation(uint32 SchemaIndex);
	SQLOperation(uint32 SchemaIndex, uint32 StatementIndex)
	{

	}
	~SQLOperation();

	SQLOperation& operator()(uint32 SchemaIndex, uint32 StatementIndex)
	{
		return *this;
	}

	void Clear()
	{
		SQLOperationParamsArchive::ClearParams();
		SQLOperationResultSet::ClearResultSet();
		SchemaIndex = 0;
		PreparedStatementIndex = 0;
		OperationFlag = SQLOperationFlag::Neither;
	}

	// init params
	void SetConnection(SQLConnection* Connection);
	SQLOperation& SetStatement(MYSQL_STMT* Statement);
	SQLOperation& SetStatement(char* StatementString);
	SQLOperation& SetStatement(uint32 StatementIndex)
	{
		return *this;
	}
	void SetOperationFlag(SQLOperationFlag flag);
	void Execute();
	void Call();
	bool Completed();

private:

	static uint32 SizeForType(MYSQL_FIELD* field);

	bool FetchNextRow();

	void ReleaseConnection();
};