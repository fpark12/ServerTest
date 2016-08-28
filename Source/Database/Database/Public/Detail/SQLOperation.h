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

union SQLOperationStatement
{
	MYSQL_STMT* PreparedStatement;
	char* RawStringStatement;
};

class SQLOperation : public SQLOperationParamsArchive, public SQLOperationResultSet
{
	DISABLE_COPY_AND_ASSIGN(SQLOperation);

private:

	// General
	SQLConnection* Connection;
	SQLOperationStatement Statement;
	SQLOperationFlag OperationFlag;
	std::atomic<SQLOperationStatus> OperationStatus = SQLOperationStatus::None;

public:

	SQLOperation(SQLConnection* conn);
	~SQLOperation();

	void Clear()
	{
		SQLOperationParamsArchive::ClearParams();
		SQLOperationResultSet::ClearResultSet();
		Statement = { nullptr };
		OperationFlag = SQLOperationFlag::Neither;
	}

	// init params
	void SetConnection(SQLConnection* Connection);
	SQLOperation& SetStatement(MYSQL_STMT* Statement);
	SQLOperation& SetStatement(char* StatementString);
	void SetOperationFlag(SQLOperationFlag flag);
	void Execute();
	void Call();
	bool Completed();

private:

	static uint32 SizeForType(MYSQL_FIELD* field);

	bool FetchNextRow();

	void ReleaseConnection();
};