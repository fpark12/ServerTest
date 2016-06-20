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
#include <memory>
#include "Errors.h"
#include "Field.h"

#ifdef _WIN32
#include <winsock2.h>
#endif
#include <mysql.h>

class ResultSet
{
public:
	ResultSet(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount);
	~ResultSet();

	bool NextRow();
	uint64 GetRowCount() const { return rowCount; }
	uint32 GetFieldCount() const { return fieldCount; }

	Field* Fetch() const { return currentRow; }
	const Field & operator [] (uint32 index) const
	{
		ASSERT(index < fieldCount);
		return currentRow[index];
	}

protected:
	uint64 rowCount;
	Field* currentRow;
	uint32 fieldCount;

private:
	void CleanUp();
	MYSQL_RES* result;
	MYSQL_FIELD* fields;

	ResultSet(ResultSet const& right) = delete;
	ResultSet& operator=(ResultSet const& right) = delete;
};

typedef std::shared_ptr<ResultSet> QueryResult;

class PreparedResultSet
{
public:
	PreparedResultSet(MYSQL_STMT* stmt, MYSQL_RES* result, uint64 rowCount, uint32 fieldCount);
	~PreparedResultSet();

	bool NextRow();
	uint64 GetRowCount() const { return rowCount; }
	uint32 GetFieldCount() const { return fieldCount; }

	Field* Fetch() const
	{
		ASSERT(rowPosition < rowCount);
		return const_cast<Field*>(&rows[uint32(rowPosition) * fieldCount]);
	}

	Field const& operator[](uint32 index) const
	{
		ASSERT(rowPosition < rowCount);
		ASSERT(index < fieldCount);
		return rows[uint32(rowPosition) * fieldCount + index];
	}

protected:
	std::vector<Field> rows;
	uint64 rowCount;
	uint64 rowPosition;
	uint32 fieldCount;

private:
	MYSQL_BIND* rBind;
	MYSQL_STMT* stmt;
	MYSQL_RES* metadataResult;    ///< Field metadata, returned by mysql_stmt_result_metadata

	my_bool* isNull;
	unsigned long* length;

	void CleanUp();
	bool _NextRow();

	PreparedResultSet(PreparedResultSet const& right) = delete;
	PreparedResultSet& operator=(PreparedResultSet const& right) = delete;
};

typedef std::shared_ptr<PreparedResultSet> PreparedQueryResult;
