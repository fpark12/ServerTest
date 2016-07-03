#pragma once
#include <memory>
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
	uint64 GetRowCount() const { return _rowCount; }
	uint32 GetFieldCount() const { return _fieldCount; }

	Field* Fetch() const { return _currentRow; }
	const Field & operator [] (uint32 index) const
	{
		ASSERT(index < _fieldCount);
		return _currentRow[index];
	}

protected:
	uint64 _rowCount;
	Field* _currentRow;
	uint32 _fieldCount;

private:
	void CleanUp();
	MYSQL_RES* _result;
	MYSQL_FIELD* _fields;

	ResultSet(ResultSet const& right) = delete;
	ResultSet& operator=(ResultSet const& right) = delete;
};


