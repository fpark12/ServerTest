#include "SQLOperation.h"
#include "Public\Detail\SQLDatabase.h"
#include "SQLOperationParamsArchive.h"
#include "SQLOperationResultSet.h"

class SQLStatement : public SQLOperation, public SQLOperationParamsArchive, public SQLOperationResultSet
{

private:

	// statement type info
	bool IsStatementRawString;

	// statement related
	MYSQL_STMT* PreparedStatement;
	const char* RawStringStatement;

	//TODO: Statement Storage & Statement Index

public:

	SQLStatement();

	SQLStatement(uint32 _schema_idx, const char* _stmt_string);

	SQLStatement(uint32 _schema_idx, uint32 _stmt_idx);

	~SQLStatement();
	
	uint32 Execute();

	// TODO
	// if we want to keep using the same prepared stmt for next task
	// merely clean output buffer
	// and use set param to change inputs

	// the following functions are used to assign new sql instruction to the same operation object
	// the input and result buffer is automatically cleared upon reset
	// reset schema if we want to use the same operation to execute different stmt
	void ResetSchema(uint32 _schema_idx);

	void ResetStatement(const char* _stmt_string);

	void ResetStatement(uint32 _stmt_idx);

private:

	// allocate memory for param according to # of params needed in stmt
	void InitializeParamMemory();

	// init prepared stmt
	uint32 InitPreparedStatement(MYSQL* _mysql);

	// fetching the next row on result set and store it into result buffer
	bool FetchNextRow();

};