#include "ResultSet.h"
#include <Errors.h>
#include <mysql.h>
#include "ParamBind.h"

ResultSet::ResultSet(MYSQL_STMT* stmt, MYSQL_RES* result, uint64 rowCount, uint32 fieldCount) :
	RowCount(rowCount),
	RowPosition(0),
	FieldCount(fieldCount),
	Statement(stmt),
	ResultMetadata(result)
{
	ASSERT(ResultMetadata);
	ASSERT(!stmt->bind_result_done);

	//- This is where we store the (entire) resultset
	if (mysql_stmt_store_result(Statement))
	{
		//TODO TC_LOG_WARN("sql.sql", "%s:mysql_stmt_store_result, cannot bind result from MySQL server. Error: %s", __FUNCTION__, mysql_stmt_error(m_stmt));
		return;
	}

	RowCount = mysql_stmt_num_rows(Statement);

	//- This is where we prepare the buffer based on metadata
	MYSQL_FIELD* field = mysql_fetch_fields(ResultMetadata);
	std::size_t rowTotalSize = ParamBind::ParamsTotalSize(field, FieldCount);
	AllDataBuffer = new uint8[uint32(RowCount) * rowTotalSize];
	AllBindStructBuffer = new ParamBind::MySqlBindWrapper[uint32(RowCount) * FieldCount];
	RowData = new ParamBind(field, FieldCount, AllDataBuffer, rowTotalSize, AllBindStructBuffer);

	//- This is where we bind the buffer to the statement
	if (mysql_stmt_bind_result(stmt, &(RowData->MySqlBinds->Bind)))
	{
		//TODO TC_LOG_WARN("sql.sql", "%s:mysql_stmt_bind_result, cannot bind result from MySQL server. Error: %s", __FUNCTION__, mysql_stmt_error(m_stmt));
		mysql_stmt_free_result(stmt);
		// CleanUp();
		//TODO clean up row data
		return;
	}

	while (RowPosition < RowCount)
	{
		for (uint32 fieldIndex = 0; fieldIndex < FieldCount; ++fieldIndex)
		{
			unsigned long buffer_length = AllBindStructBuffer[fieldIndex].buffer_length;
			unsigned long fetched_length = *m_rBind[fieldIndex].length;
			if (!*m_rBind[fieldIndex].is_null)
			{
				void* buffer = m_stmt->bind[fieldIndex].buffer;
				switch (m_rBind[fieldIndex].buffer_type)
				{
				case MYSQL_TYPE_TINY_BLOB:
				case MYSQL_TYPE_MEDIUM_BLOB:
				case MYSQL_TYPE_LONG_BLOB:
				case MYSQL_TYPE_BLOB:
				case MYSQL_TYPE_STRING:
				case MYSQL_TYPE_VAR_STRING:
					// warning - the string will not be null-terminated if there is no space for it in the buffer
					// when mysql_stmt_fetch returned MYSQL_DATA_TRUNCATED
					// we cannot blindly null-terminate the data either as it may be retrieved as binary blob and not specifically a string
					// in this case using Field::GetCString will result in garbage
					// TODO: remove Field::GetCString and use boost::string_ref (currently proposed for TS as string_view, maybe in C++17)
					if (fetched_length < buffer_length)
						*((char*)buffer + fetched_length) = '\0';
					break;
				default:
					break;
				}

				m_rows[uint32(RowPosition) * FieldCount + fieldIndex].SetByteValue(
					buffer,
					m_rBind[fieldIndex].buffer_type,
					fetched_length);

				// move buffer pointer to next part
				m_stmt->bind[fieldIndex].buffer = (char*)buffer + rowSize;
			}
			else
			{
				m_rows[uint32(RowPosition) * FieldCount + fieldIndex].SetByteValue(
					nullptr,
					m_rBind[fieldIndex].buffer_type,
					*m_rBind[fieldIndex].length);
			}
		}
		RowPosition++;
	}
	RowPosition = 0;

	/// All data is buffered, let go of mysql c api structures
	mysql_stmt_free_result(m_stmt);
}

bool ResultSet::_NextRow()
{
	/// Only called in low-level code, namely the constructor
	/// Will iterate over every row of data and buffer it
	if (RowPosition >= RowCount)
		return false;

	int retval = mysql_stmt_fetch(m_stmt);
	return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
}

