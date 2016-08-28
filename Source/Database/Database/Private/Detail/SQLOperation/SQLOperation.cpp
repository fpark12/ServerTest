#include "SQLOperation.h"
#include "SQLConnection.h"

SQLOperation::SQLOperation(SQLConnection* Connection) :
	Connection{ 0 },
	Statement{ 0 },
	OperationFlag(SQLOperationFlag::Neither),
	SQLOperationParamsArchive(),
	SQLOperationResultSet()
{
}

SQLOperation::SQLOperation(uint32 SchemaIndex) :
	SchemaIndex(SchemaIndex),
	Connection{ 0 },
	Statement{ 0 },
	OperationFlag(SQLOperationFlag::Neither),
	SQLOperationParamsArchive(),
	SQLOperationResultSet()
{
}

SQLOperation::~SQLOperation()
{
	ClearParams();
	ClearResultSet();
}



void SQLOperation::SetConnection(SQLConnection* conn)
{
	Connection.Connection = conn;
	Connection.Connection->IsFree = false;
}

SQLOperation& SQLOperation::SetStatement(MYSQL_STMT* Statement)
{
	this->OperationFlag = SQLOperationFlag::Prepared;
	this->Statement.PreparedStatement = Statement;
	ParamCount = mysql_stmt_param_count(Statement);
	ParamBinds = new MYSQL_BIND[ParamCount];
	memset(ParamBinds, 0, sizeof(MYSQL_BIND)*ParamCount);
	ParamData = new uint64[ParamCount];
	memset(ParamData, 0, sizeof(uint64)*ParamCount);

	/// "If set to 1, causes mysql_stmt_store_result() to update the metadata MYSQL_FIELD->max_length value."
	my_bool bool_tmp = 1;
	mysql_stmt_attr_set(Statement, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);
	return *this;
}

SQLOperation& SQLOperation::SetStatement(char* StatementString)
{
	if (strchr(StatementString, '?'))
	{
		this->OperationFlag = SQLOperationFlag::RawStringPrepared;

		if (!Connection)
		{
			GConsole.Message("{}: Connection is null.", __FUNCTION__);
			return *this;
		}
		MYSQL_STMT* TempStatement = mysql_stmt_init(Connection->MySqlHandle);

		if (mysql_stmt_prepare(TempStatement, StatementString, uint32(strlen(StatementString))))
		{
			const char* err = mysql_stmt_error(TempStatement);
			GConsole.Message("{}: Error parsing prepared statement: {}.", __FUNCTION__, err);
			mysql_print_error(Connection->MySqlHandle);
			OperationStatus = SQLOperationStatus::Failed;
			return *this;
		}

		SetStatement(TempStatement);
	}
	else
	{
		this->OperationFlag = SQLOperationFlag::RawString;

		Statement.RawStringStatement = new char[strlen(StatementString) + 1];
		memcpy(Statement.RawStringStatement, StatementString, strlen(StatementString) + 1);
	}

	return *this;
}

void SQLOperation::SetOperationFlag(SQLOperationFlag flag)
{
	OperationFlag = flag;
}

void SQLOperation::Execute()
{
	if (!Connection)
	{
		GConsole.Message("{}: Connection is null.", __FUNCTION__);
		return;
	}

	MYSQL_RES* resultMetaData;
	if (OperationFlag == SQLOperationFlag::Prepared)
	{
		if (ParamCount)
		{
			mysql_stmt_bind_param(Statement.PreparedStatement, ParamBinds);
		}

		if (mysql_stmt_execute(Statement.PreparedStatement))
		{
			mysql_print_error(Connection->MySqlHandle);
			OperationStatus = SQLOperationStatus::Failed;
			return;
		}

		if (mysql_stmt_store_result(Statement.PreparedStatement))
		{
			mysql_print_error(Connection->MySqlHandle);
			OperationStatus = SQLOperationStatus::Failed;
			return;
		}

		resultMetaData = mysql_stmt_result_metadata(Statement.PreparedStatement);
	}
	else if (OperationFlag == SQLOperationFlag::RawString)
	{
		if (mysql_real_query(Connection->MySqlHandle, Statement.RawStringStatement, strlen(Statement.RawStringStatement)))
		{
			mysql_print_error(Connection->MySqlHandle);
			OperationStatus = SQLOperationStatus::Failed;
			return;
		}
		
		resultMetaData = mysql_store_result(Connection->MySqlHandle);
	}

	if (!resultMetaData)
	{
		if (mysql_field_count(Connection->MySqlHandle) != 0)
		{
			mysql_print_error(Connection->MySqlHandle);
		}
		return;
	}

	FieldCount = mysql_num_fields(resultMetaData);
	RowCount = mysql_num_rows(resultMetaData);

	if (FieldCount)
	{
		// Init data serialization
		FieldBinds = new MYSQL_BIND[FieldCount];
		memset(FieldBinds, 0, sizeof(MYSQL_BIND)*FieldCount);
		RowData = new uint64[FieldCount];
		memset(RowData, 0, sizeof(uint64)*FieldCount);

		// get metadata
		MYSQL_FIELD* resultDataFields = mysql_fetch_fields(resultMetaData);

		// bind result for fetching
		if (Statement.PreparedStatement->bind_result_done)
		{
			delete[] Statement.PreparedStatement->bind->length;
			delete[] Statement.PreparedStatement->bind->is_null;
		}

		for (uint32 i = 0; i < FieldCount; ++i)
		{

			uint32 size = SizeForType(&resultDataFields[i]);

			if (resultDataFields[i].type == MYSQL_TYPE_VAR_STRING ||
				resultDataFields[i].type == MYSQL_TYPE_BLOB)
			{
				char* fieldBuffer = new char[size];
				RowData[i] = reinterpret_cast<uint64&>(fieldBuffer);
				SetParamBind(&FieldBinds[i], resultDataFields[i].type,
					fieldBuffer, false, size, size);
			}
			else
			{
				SetParamBind(&FieldBinds[i], resultDataFields[i].type,
					&RowData[i], !!(resultDataFields[i].flags & UNSIGNED_FLAG));
			}
		}

		mysql_stmt_bind_result(Statement.PreparedStatement, FieldBinds);
		ResultSetData = new uint64[RowCount*FieldCount];

		uint32 rowIndex = 0;
		while (FetchNextRow())
		{
			for (uint32 fieldIndex = 0; fieldIndex < FieldCount; ++fieldIndex)
			{
				if (FieldBinds[fieldIndex].buffer_type == MYSQL_TYPE_BLOB ||
					FieldBinds[fieldIndex].buffer_type == MYSQL_TYPE_VAR_STRING)
				{
					uint32 fieldBufferSize = FieldBinds[fieldIndex].buffer_length;
					char* fieldBuffer = new char[fieldBufferSize];
					memcpy(fieldBuffer, (void*)RowData[fieldIndex], fieldBufferSize);
					ResultSetData[rowIndex*FieldCount + fieldIndex] = reinterpret_cast<uint64&>(fieldBuffer);
				}
				else
				{
					memcpy(&ResultSetData[rowIndex*FieldCount + fieldIndex],
						&RowData[fieldIndex], sizeof(uint64));
				}
			}
			++rowIndex;
		}
		mysql_stmt_free_result(Statement.PreparedStatement);
	}

	if (OperationFlag == SQLOperationFlag::RawStringPrepared)
	{
		delete Statement.PreparedStatement;
		Statement = { nullptr };
	}
}

void SQLOperation::Call()
{
	Execute();
	OperationStatus = SQLOperationStatus::Success;
	ReleaseConnection();
}

bool SQLOperation::Completed()
{
	return OperationStatus == SQLOperationStatus::Success;
}

uint32 SQLOperation::SizeForType(MYSQL_FIELD* field)
{
	switch (field->type)
	{
	case MYSQL_TYPE_NULL:
		return RC_SUCCESS;
	case MYSQL_TYPE_TINY:
		return RC_FAILED;
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_SHORT:
		return 2;
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
	case MYSQL_TYPE_FLOAT:
		return 4;
	case MYSQL_TYPE_DOUBLE:
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_BIT:
		return 8;

	case MYSQL_TYPE_TIMESTAMP:
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_DATETIME:
		return sizeof(MYSQL_TIME);

	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
		return field->max_length + 1;

	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_NEWDECIMAL:
		return 64;

	case MYSQL_TYPE_GEOMETRY:
		/*
		Following types are not sent over the wire:
		MYSQL_TYPE_ENUM:
		MYSQL_TYPE_SET:
		*/
	default:
		//TODO error log
		return RC_SUCCESS;
	}
}

bool SQLOperation::FetchNextRow()
{
	int retval = mysql_stmt_fetch(Statement.PreparedStatement);
	return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
}

void SQLOperation::ReleaseConnection()
{
	if (Connection)
	{
		bool Expected = false;
		Connection->IsFree.compare_exchange_strong(Expected, true);
		Connection = nullptr;
	}
}
