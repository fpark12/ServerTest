/*
#include "PreparedStatement.h"
#include "ParamBind.h"
#include "Errors.h"

PreparedStatement::PreparedStatement(MYSQL_STMT* stmt)
{
	ParamCount = mysql_stmt_param_count(stmt);
	ParamSetMask = 0x00000000;
	MySqlBindHandle = new MYSQL_BIND[ParamCount];
	memset(MySqlBindHandle, 0, sizeof(MYSQL_BIND) * ParamCount);

	my_bool bool_tmp = 1;
	mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
}

PreparedStatement::~PreparedStatement()
{
	ClearParameters();
	if (MySqlStatementHandle->bind_result_done)
	{
		delete[] MySqlStatementHandle->bind->length;
		delete[] MySqlStatementHandle->bind->is_null;
	}
	mysql_stmt_close(MySqlStatementHandle);
	delete[] MySqlBindHandle;
}

void PreparedStatement::ClearParameters()
{
	for (uint32 i = 0; i < ParamCount; ++i)
	{
		delete MySqlBindHandle[i].length;
		MySqlBindHandle[i].length = NULL;
		delete[](char*) MySqlBindHandle[i].buffer;
		MySqlBindHandle[i].buffer = NULL;
		ParamSetMask &= ~(0x00000001 << i);
	}
}

void PreparedStatement::SetValue(MYSQL_BIND* param, enum_field_types type, const void* value, uint32 len, bool isUnsigned)
{
	param->buffer_type = type;
	delete[] static_cast<char *>(param->buffer);
	param->buffer = new char[len];
	param->buffer_length = 0;
	param->is_null_value = 0;
	param->length = NULL;               // Only != NULL for strings
	param->is_unsigned = isUnsigned;

	memcpy(param->buffer, value, len);
}*/