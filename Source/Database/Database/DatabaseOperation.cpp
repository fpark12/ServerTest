#include "DatabaseOperation.h"

void DatabaseOperation::SetStatement(MYSQL_STMT* stmt)
{
	MySqlStatementHandle = stmt;
	ParamCount = mysql_stmt_param_count(stmt);
	ParamSetMask = 0x00000000;
	ParamBindHandle = new MYSQL_BIND[ParamCount];
	memset(ParamBindHandle, 0, sizeof(MYSQL_BIND)*ParamCount);

	/// "If set to 1, causes mysql_stmt_store_result() to update the metadata MYSQL_FIELD->max_length value."
	my_bool bool_tmp = 1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
}

void DatabaseOperation::SetOperationFlag(SqlOperationFlag flag)
{
	OperationFlag = flag;
}
