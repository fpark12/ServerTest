/*
* Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <mysql.h>
#include "QueryResult.h"

ResultSet::ResultSet(MYSQL_RES *result, MYSQL_FIELD *fields, uint64 rowCount, uint32 fieldCount) :
	rowCount(rowCount),
	fieldCount(fieldCount),
	result(result),
	fields(fields)
{
	currentRow = new Field[fieldCount];
}

PreparedResultSet::PreparedResultSet(MYSQL_STMT* stmt, MYSQL_RES *result, uint64 rowCount, uint32 fieldCount) :
	rowCount(rowCount),
	rowPosition(0),
	fieldCount(fieldCount),
	rBind(NULL),
	stmt(stmt),
	metadataResult(result),
	isNull(NULL),
	length(NULL)
{
	if (!metadataResult)
		return;

	if (stmt->bind_result_done)
	{
		delete[] stmt->bind->length;
		delete[] stmt->bind->is_null;
	}

	rBind = new MYSQL_BIND[fieldCount];
	isNull = new my_bool[fieldCount];
	length = new unsigned long[fieldCount];

	memset(isNull, 0, sizeof(my_bool) * fieldCount);
	memset(rBind, 0, sizeof(MYSQL_BIND) * fieldCount);
	memset(length, 0, sizeof(unsigned long) * fieldCount);

	//- This is where we store the (entire) resultset
	if (mysql_stmt_store_result(stmt))
	{
		// TODO
		// TC_LOG_WARN("sql.sql", "%s:mysql_stmt_store_result, cannot bind result from MySQL server. Error: %s", __FUNCTION__, mysql_stmt_error(stmt));
		delete[] rBind;
		delete[] isNull;
		delete[] length;
		return;
	}

	rowCount = mysql_stmt_num_rows(stmt);

	//- This is where we prepare the buffer based on metadata
	MYSQL_FIELD* field = mysql_fetch_fields(metadataResult);
	std::size_t rowSize = 0;
	for (uint32 i = 0; i < fieldCount; ++i)
	{
		uint32 size = Field::SizeForType(&field[i]);
		rowSize += size;

		rBind[i].buffer_type = field[i].type;
		rBind[i].buffer_length = size;
		rBind[i].length = &length[i];
		rBind[i].is_null = &isNull[i];
		rBind[i].error = NULL;
		rBind[i].is_unsigned = field[i].flags & UNSIGNED_FLAG;
	}

	char* dataBuffer = new char[rowSize * rowCount];
	for (uint32 i = 0, offset = 0; i < fieldCount; ++i)
	{
		rBind[i].buffer = dataBuffer + offset;
		offset += rBind[i].buffer_length;
	}

	//- This is where we bind the buffer to the statement
	if (mysql_stmt_bind_result(stmt, rBind))
	{
		// TC_LOG_WARN("sql.sql", "%s:mysql_stmt_bind_result, cannot bind result from MySQL server. Error: %s", __FUNCTION__, mysql_stmt_error(stmt));
		mysql_stmt_free_result(stmt);
		CleanUp();
		delete[] isNull;
		delete[] length;
		return;
	}

	rows.resize(uint32(rowCount) * fieldCount);
	while (_NextRow())
	{
		for (uint32 fIndex = 0; fIndex < fieldCount; ++fIndex)
		{
			unsigned long buffer_length = rBind[fIndex].buffer_length;
			unsigned long fetched_length = *rBind[fIndex].length;
			if (!*rBind[fIndex].is_null)
			{
				void* buffer = stmt->bind[fIndex].buffer;
				switch (rBind[fIndex].buffer_type)
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

				rows[uint32(rowPosition) * fieldCount + fIndex].SetByteValue(
					buffer,
					rBind[fIndex].buffer_type,
					fetched_length);

				// move buffer pointer to next part
				stmt->bind[fIndex].buffer = (char*)buffer + rowSize;
			}
			else
			{
				rows[uint32(rowPosition) * fieldCount + fIndex].SetByteValue(
					nullptr,
					rBind[fIndex].buffer_type,
					*rBind[fIndex].length);
			}

		}
		rowPosition++;
	}
	rowPosition = 0;

	/// All data is buffered, let go of mysql c api structures
	mysql_stmt_free_result(stmt);
}

ResultSet::~ResultSet()
{
	CleanUp();
}

PreparedResultSet::~PreparedResultSet()
{
	CleanUp();
}

bool ResultSet::NextRow()
{
	MYSQL_ROW row;

	if (!result)
		return false;

	row = mysql_fetch_row(result);
	if (!row)
	{
		CleanUp();
		return false;
	}

	unsigned long* lengths = mysql_fetch_lengths(result);
	if (!lengths)
	{
		// TC_LOG_WARN("sql.sql", "%s:mysql_fetch_lengths, cannot retrieve value lengths. Error %s.", __FUNCTION__, mysql_error(result->handle));
		CleanUp();
		return false;
	}

	for (uint32 i = 0; i < fieldCount; i++)
		currentRow[i].SetStructuredValue(row[i], fields[i].type, lengths[i]);

	return true;
}

bool PreparedResultSet::NextRow()
{
	/// Only updates the m_rowPosition so upper level code knows in which element
	/// of the rows vector to look
	if (++rowPosition >= rowCount)
		return false;

	return true;
}

bool PreparedResultSet::_NextRow()
{
	/// Only called in low-level code, namely the constructor
	/// Will iterate over every row of data and buffer it
	if (rowPosition >= rowCount)
		return false;

	int retval = mysql_stmt_fetch(stmt);
	return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
}

void ResultSet::CleanUp()
{
	if (currentRow)
	{
		delete[] currentRow;
		currentRow = NULL;
	}

	if (result)
	{
		mysql_free_result(result);
		result = NULL;
	}
}

void PreparedResultSet::CleanUp()
{
	if (metadataResult)
		mysql_free_result(metadataResult);

	if (rBind)
	{
		delete[](char*)rBind->buffer;
		delete[] rBind;
		rBind = nullptr;
	}
}
