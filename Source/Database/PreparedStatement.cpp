#include "PreparedStatement.h"
#include "Bind.h"
#include "Errors.h"

PreparedStatement::PreparedStatement(StatementStorage _storage, uint32 _index, Bind* _paramTypeInfo)
	: Storage(_storage), Index(_index)
{
	SetParametersypeInfo(_paramTypeInfo);
}

void PreparedStatement::ClearParameters()
{
	ParametersBuffer->ClearParameters();
}

void PreparedStatement::SetParametersypeInfo(Bind* _parameter)
{
	ParametersBuffer = std::unique_ptr<Bind>(std::move(_parameter));
}

void PreparedStatement::SetBool(const uint8 index, const bool value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_BOOL);
	ParametersBuffer->SetParam<bool>(index, std::move(value));
}

void PreparedStatement::setUInt8(const uint8 index, const uint8 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_UI8);
	ParametersBuffer->SetParam<uint8>(index, std::move(value));
}

void PreparedStatement::setUInt16(const uint8 index, const uint16 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_UI16);
	ParametersBuffer->SetParam<uint16>(index, std::move(value));
}

void PreparedStatement::setUInt32(const uint8 index, const uint32 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_UI32);
	ParametersBuffer->SetParam<uint32>(index, std::move(value));
}

void PreparedStatement::setUInt64(const uint8 index, const uint64 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_UI64);
	ParametersBuffer->SetParam<uint64>(index, std::move(value));
}

void PreparedStatement::setInt8(const uint8 index, const int8 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_I8);
	ParametersBuffer->SetParam<int8>(index, std::move(value));
}

void PreparedStatement::setInt16(const uint8 index, const int16 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_I16);
	ParametersBuffer->SetParam<int16>(index, std::move(value));
}

void PreparedStatement::setInt32(const uint8 index, const int32 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_I32);
	ParametersBuffer->SetParam<int32>(index, std::move(value));
}

void PreparedStatement::setInt64(const uint8 index, const int64 value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_I64);
	ParametersBuffer->SetParam<int64>(index, std::move(value));
}

void PreparedStatement::setFloat(const uint8 index, const float value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_FLOAT);
	ParametersBuffer->SetParam<float>(index, std::move(value));
}

void PreparedStatement::setDouble(const uint8 index, const double value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_DOUBLE);
	ParametersBuffer->SetParam<double>(index, std::move(value));
}

void PreparedStatement::setString(const uint8 index, const char* value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_STRING);
	ParametersBuffer->SetParam<const char*>(index, std::move(value));
}

void PreparedStatement::setString(const uint8 index, const std::string& value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_STRING);
	ParametersBuffer->SetParam<const char*>(index, value.c_str());
}

void PreparedStatement::setBinary(const uint8 index, const std::vector<uint8>& value)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_BINARY);
	ParametersBuffer->SetParam<const char*>(index, (const char*)value.data());
}

void PreparedStatement::setNull(const uint8 index)
{
	ASSERT(ParametersBuffer->GetParamType(index) == QueryParamType::TYPE_NULL);
	ParametersBuffer->SetParam<void*>(index, NULL);
}