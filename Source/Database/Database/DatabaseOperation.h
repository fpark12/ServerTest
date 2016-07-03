#pragma once

class DatabaseOperation
{

private:

	PreparedStatement* SqlStatement;
	/* ResultSet class should support both sync & async result */
	ResultSet* QueryResult;
	DatabaseConnection* Connection;

	DISALLOW_COPY(DatabaseOperation);

public:

	bool SetStatement(PreparedStatement* stmt)
	{
		SqlStatement = stmt;
	}

	bool SetConnection(DatabaseConnection* conn)
	{
		Connection = conn;
	}

	template<typename T>
	bool SetParam(uint8 index, T value);

	ResultSet* GetResult()
	{

	}
	
};