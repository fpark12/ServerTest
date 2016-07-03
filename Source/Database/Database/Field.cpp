#include "Field.h"

int32 Field::SizeForType(MYSQL_FIELD* field)
{
	switch (field->type)
	{
	case MYSQL_TYPE_NULL:
		return 0;
	case MYSQL_TYPE_TINY:
		return 1;
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
		return MYSQL_TYPE_NOT_RECOGNIZED_ERROR;
	}
}

bool Field::IsNumeric() const
{
	return (data.type == MYSQL_TYPE_TINY ||
		data.type == MYSQL_TYPE_SHORT ||
		data.type == MYSQL_TYPE_INT24 ||
		data.type == MYSQL_TYPE_LONG ||
		data.type == MYSQL_TYPE_FLOAT ||
		data.type == MYSQL_TYPE_DOUBLE ||
		data.type == MYSQL_TYPE_LONGLONG);
}

bool Field::IsBlobOrText() const
{
	return (data.type == MYSQL_TYPE_TINY_BLOB ||
		data.type == MYSQL_TYPE_MEDIUM_BLOB ||
		data.type == MYSQL_TYPE_LONG_BLOB ||
		data.type == MYSQL_TYPE_BLOB ||
		data.type == MYSQL_TYPE_STRING ||
		data.type == MYSQL_TYPE_VAR_STRING);
}

bool Field::IsType(enum_field_types type) const
{
	return data.type == type;
}

bool Field::SetValue(uint32 _length, void* _value, enum_field_types _type)
{
	data.length = _length;
	data.type = _type;
	data.value = new char[_length];
	memcpy(data.value, _value, data.length);
}

bool Field::SetValue(void* _value, enum_field_types _type)
{
	data.length = 0;
	data.type = _type;
	data.value = _value;
}

void Field::CleanUp()
{
	data.value = nullptr;
}
