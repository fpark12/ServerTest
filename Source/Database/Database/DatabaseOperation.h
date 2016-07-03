#pragma once
#include <mysql.h>

enum StatementStorage
{
	Login,
	World,
	Character,
	Hotfix,
	None
};

enum SqlOperationFlag
{
	Sync,
	Async,
	Both
};

class DatabaseOperation
{

private:

	// General
	MYSQL_STMT* MySqlStatementHandle;
	SqlOperationFlag OperationFlag;

	// Params: for input
	uint32 ParamCount;
	uint32 ParamSetMask;	// A max of 32 param is allowed for each query
	MYSQL_BIND* ParamBindHandle;
	uint64* ParamDataSerialization;
	enum_field_types* ParamTypeInfo;
	StatementStorage Storage;

	// Field: for output


	// Database Connection
	

	DISALLOW_COPY(DatabaseOperation);

public:

	DatabaseOperation() {}

	// void SetConnection(DatabaseConnection* conn);
	void SetStatement(MYSQL_STMT* stmt);
	void SetOperationFlag(SqlOperationFlag flag);

	template <typename T>
	void SetParam(uint8 index, T& value)
	{
		ParamDataSerialization[index] = reinterpret_cast<uint64&>(value);
		// ParamTypeInfo[index] = typeid(T);

		switch (T)
		{
		case bool:

		default:
			break;
		}
	}
};