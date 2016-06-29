#pragma once
//#include "Database.pch.h"
#include <mysql.h>

namespace Utility
{

	enum QueryParamCPPType
	{
		TYPE_BOOL,
		TYPE_UI8,
		TYPE_UI16,
		TYPE_UI32,
		TYPE_UI64,
		TYPE_I8,
		TYPE_I16,
		TYPE_I32,
		TYPE_I64,
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_STRING,
		TYPE_BINARY,
		TYPE_NULL
	};
	
	struct QueryParamDetails
	{
		QueryParamCPPType Type;
		std::size_t MaxDataLength;
	};
	
	struct MySqlBindWrapper
	{
		MYSQL_BIND Bind;
		unsigned long FieldLength;
		my_bool NullIndicator;
	
		static MySqlBindWrapper* CreateArrayOfMySqlBinds(uint64 count)
		{
			MySqlBindWrapper* createdBindsArray = new MySqlBindWrapper[count];
			for (int i = 0; i < count; ++i)
			{
				createdBindsArray[i].Bind.length = &(createdBindsArray[i].FieldLength);
				createdBindsArray[i].Bind.is_null = &(createdBindsArray[i].NullIndicator);
			}
			return createdBindsArray;
		}
	};

}
