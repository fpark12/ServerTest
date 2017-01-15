#include "SQLStatement.h"

SQLStatement::SQLStatement() : IsStatementRawString(false)
{

}

SQLStatement::SQLStatement(uint32 _schema_idx, const char* _stmt_string) :
	IsStatementRawString(!strchr(_stmt_string, '?')),
	RawStringStatement(_stmt_string)
{
	SQLOperation::SchemaIndex = _schema_idx;
}

SQLStatement::SQLStatement(uint32 _schema_idx, uint32 _stmt_idx)
{
	// TODO get stmt from stmt repo

	SQLOperation::SchemaIndex = _schema_idx;
}

SQLStatement::~SQLStatement()
{
	ClearInputOutputBuffer();
}

uint32 SQLStatement::Execute()
{
	// get connection from corresponding connection pool
	SQLConnection* conn = GDatabase.GetAvaliableSQLConnection(SchemaIndex);
	MYSQL* mySql = SQLOperation::GetMySQLHandle(conn);
	// result storage
	MYSQL_RES* resultMetaData;

	// test validity of connection
	if (!conn)
	{
		GConsole.Message("{}: Connection to target schema unavailable.", __FUNCTION__);
		SQLOperation::OperationStatus = SQLOperation::SQLOperationStatus::Failed;
		return RC_FAILED;
	}

	// deal with raw string statement first
	if (IsStatementRawString)
	{
		if (mysql_real_query(mySql, RawStringStatement, strlen(RawStringStatement)))
		{
			const char* err = mysql_error(mySql);
			GConsole.Message("{}: Error executing raw string statement: {}", __FUNCTION__, err);
			OperationStatus = SQLOperation::SQLOperationStatus::Failed;
			return RC_FAILED;
		}

		resultMetaData = mysql_store_result(mySql);
	}
	else
	{
		InitPreparedStatement(mySql);

		// eceute prepared stmt
		if (mysql_stmt_execute(PreparedStatement))
		{
			const char* err = mysql_error(mySql);
			GConsole.Message("{}: Error executing prepared statement: {}.", __FUNCTION__, err);
			OperationStatus = SQLOperation::SQLOperationStatus::Failed;
			return RC_FAILED;
		}

		// store the result if any
		if (mysql_stmt_store_result(PreparedStatement))
		{
			const char* err = mysql_error(mySql);
			GConsole.Message("{}: Error storing result of prepared statement: {}.", __FUNCTION__, err);
			OperationStatus = SQLOperation::SQLOperationStatus::Failed;
			return RC_FAILED;
		}

		// get the result, could get NULL
		resultMetaData = mysql_stmt_result_metadata(PreparedStatement);
	}

	if (!resultMetaData)
	{
		// result is expected
		if (mysql_field_count(mySql))
		{
			const char* err = mysql_error(mySql);
			GConsole.Message("{}: Expected result from prepared statement but got none: {}.", __FUNCTION__, err);
			OperationStatus = SQLOperationStatus::Failed;
			return RC_FAILED;
		}
		else // result is not expected
		{
			OperationStatus = SQLOperationStatus::Success;
			return RC_SUCCESS;
		}
	}

	SQLOperationResultSet::FieldCount = mysql_num_fields(resultMetaData);
	SQLOperationResultSet::RowCount = mysql_num_rows(resultMetaData);

	if (SQLOperationResultSet::FieldCount)
	{
		// Init data serialization
		SQLOperationResultSet::FieldBinds = new MYSQL_BIND[SQLOperationResultSet::FieldCount];
		memset(SQLOperationResultSet::FieldBinds, 0, sizeof(MYSQL_BIND) * SQLOperationResultSet::FieldCount);
		SQLOperationResultSet::RowData = new uint64[SQLOperationResultSet::FieldCount];
		memset(SQLOperationResultSet::RowData, 0, sizeof(uint64) * SQLOperationResultSet::FieldCount);

		// get metadata
		MYSQL_FIELD* resultDataFields = mysql_fetch_fields(resultMetaData);

		// bind result for fetching
		if (PreparedStatement->bind_result_done)
		{
			delete[] PreparedStatement->bind->length;
			delete[] PreparedStatement->bind->is_null;
		}

		// get a row of data
		for (uint32 i = 0; i < FieldCount; ++i)
		{

			uint32 size = SQLOperationResultSet::SizeForType(&resultDataFields[i]);

			if (resultDataFields[i].type == MYSQL_TYPE_VAR_STRING ||
				resultDataFields[i].type == MYSQL_TYPE_BLOB)
			{
				char* fieldBuffer = new char[size];
				SQLOperationResultSet::RowData[i] = reinterpret_cast<uint64&>(fieldBuffer);
				SetParamBind(&SQLOperationResultSet::FieldBinds[i], resultDataFields[i].type,
					fieldBuffer, false, size, size);
			}
			else
			{
				SetParamBind(&SQLOperationResultSet::FieldBinds[i], resultDataFields[i].type,
					&SQLOperationResultSet::RowData[i], !!(resultDataFields[i].flags & UNSIGNED_FLAG));
			}
		}

		mysql_stmt_bind_result(PreparedStatement, SQLOperationResultSet::FieldBinds);
		SQLOperationResultSet::ResultSetData = new uint64[SQLOperationResultSet::RowCount * SQLOperationResultSet::FieldCount];

		uint32 rowIndex = 0;
		while (FetchNextRow())
		{
			for (uint32 fieldIndex = 0; fieldIndex < FieldCount; ++fieldIndex)
			{
				if (SQLOperationResultSet::FieldBinds[fieldIndex].buffer_type == MYSQL_TYPE_BLOB ||
					SQLOperationResultSet::FieldBinds[fieldIndex].buffer_type == MYSQL_TYPE_VAR_STRING)
				{
					uint32 fieldBufferSize = SQLOperationResultSet::FieldBinds[fieldIndex].buffer_length;
					char* fieldBuffer = new char[fieldBufferSize];
					memcpy(fieldBuffer, (void*)SQLOperationResultSet::RowData[fieldIndex], fieldBufferSize);
					SQLOperationResultSet::ResultSetData[rowIndex*FieldCount + fieldIndex] = reinterpret_cast<uint64&>(fieldBuffer);
				}
				else
				{
					memcpy(&SQLOperationResultSet::ResultSetData[rowIndex * SQLOperationResultSet::FieldCount + fieldIndex],
						&SQLOperationResultSet::RowData[fieldIndex], sizeof(uint64));
				}
			}
			++rowIndex;
		}
		mysql_stmt_free_result(PreparedStatement);
	}

	// clear prepared stmt ptr
	if (!IsStatementRawString)
	{
		delete PreparedStatement;
		PreparedStatement = nullptr;
	}

	// release connection
	if (conn)
	{
		SQLOperation::FreeUpConnection(conn);
		conn = nullptr;
	}

	OperationStatus = SQLOperation::SQLOperationStatus::Success;
	return RC_SUCCESS;
}

void SQLStatement::ResetSchema(uint32 _schema_idx)
{
	ClearInputOutputBuffer();
	SQLOperation::SchemaIndex = _schema_idx;
}

void SQLStatement::ResetStatement(const char* _stmt_string)
{
	ClearInputOutputBuffer();
	IsStatementRawString = !strchr(_stmt_string, '?');
	RawStringStatement = _stmt_string;
}

void SQLStatement::ResetStatement(uint32 _stmt_idx)
{
	ClearInputOutputBuffer();
	// TODO stmt idx api
}

void SQLStatement::InitializeParamMemory()
{
	SQLOperationParamsArchive::ParamCount = mysql_stmt_param_count(PreparedStatement);
	if (SQLOperationParamsArchive::ParamCount)
	{
		SQLOperationParamsArchive::ParamBinds = new MYSQL_BIND[SQLOperationParamsArchive::ParamCount];
		memset(ParamBinds, 0, sizeof(MYSQL_BIND) * SQLOperationParamsArchive::ParamCount);
		SQLOperationParamsArchive::ParamData = new uint64[SQLOperationParamsArchive::ParamCount];
		memset(ParamData, 0, sizeof(uint64) * SQLOperationParamsArchive::ParamCount);
	}
}

uint32 SQLStatement::InitPreparedStatement(MYSQL* _mysql)
{
	MYSQL_STMT* tempStmt = mysql_stmt_init(_mysql);

	if (mysql_stmt_prepare(tempStmt, RawStringStatement, uint32(strlen(RawStringStatement))))
	{
		const char* err = mysql_stmt_error(tempStmt);
		GConsole.Message("{}: Error parsing prepared statement: {}.", __FUNCTION__, err);
		SQLOperation::OperationStatus = SQLOperationStatus::Failed;
		return RC_FAILED;
	}

	PreparedStatement = tempStmt;
	InitializeParamMemory();

	/// "If set to 1, causes mysql_stmt_store_result() to update the metadata MYSQL_FIELD->max_length value."
	my_bool bool_tmp = 1;
	mysql_stmt_attr_set(PreparedStatement, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
	// bind param buffer
	if (SQLOperationParamsArchive::ParamCount)
	{
		mysql_stmt_bind_param(PreparedStatement, SQLOperationParamsArchive::ParamBinds);
	}
	
	return RC_SUCCESS;
}

bool SQLStatement::FetchNextRow()
{
	int retval = mysql_stmt_fetch(PreparedStatement);
	return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
}

void SQLStatement::ClearInputOutputBuffer()
{
	SQLOperationParamsArchive::ClearParams();
	SQLOperationResultSet::ClearResultSet();
}
