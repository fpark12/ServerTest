#pragma once
#include "Define.h"

class MYSQL_STMT;
class MYSQL_RES;

class ResultSet
{
public:
	ResultSet(MYSQL_STMT* stmt, MYSQL_RES* result, uint64 rowCount, uint32 fieldCount);
	~ResultSet();

	bool NextRow();
	uint64 GetRowCount() const { return RowCount; }
	uint32 GetFieldCount() const { return FieldCount; }

	/*
	Field* Fetch() const
	{
		ASSERT(m_rowPosition < m_rowCount);
		return const_cast<Field*>(&m_rows[uint32(m_rowPosition) * m_fieldCount]);
	}

	Field const& operator[](uint32 index) const
	{
		ASSERT(m_rowPosition < m_rowCount);
		ASSERT(index < m_fieldCount);
		return m_rows[uint32(m_rowPosition) * m_fieldCount + index];
	}
	*/

protected:
	ParamBind* RowData;
	uint8* AllDataBuffer;
	ParamBind::MySqlBindWrapper* AllBindStructBuffer;
	MYSQL_STMT* Statement;
	uint64 RowCount;
	uint64 RowPosition;
	uint32 FieldCount;

private:
	MYSQL_RES* ResultMetadata;    ///< Field metadata, returned by mysql_stmt_result_metadata

	void CleanUp();
	bool _NextRow();

	DISALLOW_COPY(ResultSet);
};