#pragma once
#include <mysql.h>
#include <string>
#include <memory>
#include <utility>
#include "Define.h"

class Bind;

enum StatementStorage
{
	Login,
	World,
	Character,
	Hotifx
};

class PreparedStatement
{
	// friend class declaration
public:
	PreparedStatement(StatementStorage _storage, uint32 _index, Bind* _paramTypeInfo);
	~PreparedStatement();

	// Clear prepared statement parameters, have no effect if this instance is a raw string command
	void ClearParameters();
	// Bind prepared statement parameters, have no effect if this instance is a raw string command
	void SetBool(const uint8 index, const bool value);
	void setUInt8(const uint8 index, const uint8 value);
	void setUInt16(const uint8 index, const uint16 value);
	void setUInt32(const uint8 index, const uint32 value);
	void setUInt64(const uint8 index, const uint64 value);
	void setInt8(const uint8 index, const int8 value);
	void setInt16(const uint8 index, const int16 value);
	void setInt32(const uint8 index, const int32 value);
	void setInt64(const uint8 index, const int64 value);
	void setFloat(const uint8 index, const float value);
	void setDouble(const uint8 index, const double value);
	void setString(const uint8 index, const char* value);
	void setString(const uint8 index, const std::string& value);
	void setBinary(const uint8 index, const std::vector<uint8>& value);
	void setNull(const uint8 index);

private:

	void SetParametersypeInfo(Bind* _parameter);

protected:

private:
	StatementStorage Storage;
	uint32 Index;
	std::unique_ptr<Bind> ParametersBuffer;
	MYSQL_STMT* MySqlStatementHandle;

	/* When the instance is prepared statment, this is a pattern string (sql command with ?)
	* And if the instance is a raw string query, this is a complete sql command in raw string
	*/
	std::string SqlQueryString;
	bool IsRawStringCommand;

	bool CheckValidIndex(uint8 index);
	std::string getQueryString(std::string const& sqlPattern) const;

	DISALLOW_COPY(PreparedStatement);
private:
	void setValue(MYSQL_BIND* param, enum_field_types type, const void* value, uint32 len, bool isUnsigned);

private:
	uint32 m_paramCount;
	std::vector<bool> m_paramsSet;
	MYSQL_BIND* m_bind;
};

//- Class of which the instances are unique per MySQLConnection
//- access to these class objects is only done when a prepared statement task
//- is executed.
class TC_DATABASE_API MySQLPreparedStatement
{
	friend class MySQLConnection;
	friend class PreparedStatement;

public:
	MySQLPreparedStatement(MYSQL_STMT* stmt);
	~MySQLPreparedStatement();

	void setBool(const uint8 index, const bool value);
	void setUInt8(const uint8 index, const uint8 value);
	void setUInt16(const uint8 index, const uint16 value);
	void setUInt32(const uint8 index, const uint32 value);
	void setUInt64(const uint8 index, const uint64 value);
	void setInt8(const uint8 index, const int8 value);
	void setInt16(const uint8 index, const int16 value);
	void setInt32(const uint8 index, const int32 value);
	void setInt64(const uint8 index, const int64 value);
	void setFloat(const uint8 index, const float value);
	void setDouble(const uint8 index, const double value);
	void setBinary(const uint8 index, const std::vector<uint8>& value, bool isString);
	void setNull(const uint8 index);

protected:
	MYSQL_STMT* GetSTMT() { return m_Mstmt; }
	MYSQL_BIND* GetBind() { return m_bind; }
	PreparedStatement* m_stmt;
	void ClearParameters();
	bool CheckValidIndex(uint8 index);
	std::string getQueryString(std::string const& sqlPattern) const;

private:
	void setValue(MYSQL_BIND* param, enum_field_types type, const void* value, uint32 len, bool isUnsigned);

private:
	MYSQL_STMT* m_Mstmt;
	uint32 m_paramCount;
	std::vector<bool> m_paramsSet;
	MYSQL_BIND* m_bind;

	MySQLPreparedStatement(MySQLPreparedStatement const& right) = delete;
	MySQLPreparedStatement& operator=(MySQLPreparedStatement const& right) = delete;
};

typedef std::future<PreparedQueryResult> PreparedQueryResultFuture;
typedef std::promise<PreparedQueryResult> PreparedQueryResultPromise;

//- Lower-level class, enqueuable operation
class TC_DATABASE_API PreparedStatementTask : public SQLOperation
{
public:
	PreparedStatementTask(PreparedStatement* stmt, bool async = false);
	~PreparedStatementTask();

	bool Execute() override;
	PreparedQueryResultFuture GetFuture() { return m_result->get_future(); }

protected:
	PreparedStatement* m_stmt;
	bool m_has_result;
	PreparedQueryResultPromise* m_result;
};