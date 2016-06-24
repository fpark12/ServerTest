#pragma once
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include "Define.h"

class Bind;

enum StatementStorage
{
	Login,
	World,
	Character,
	Hotfix
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
