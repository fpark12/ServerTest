#include "DatabaseUtility.h"


int main()
{
	Utility::MySqlBindWrapper* wrapperArray = Utility::MySqlBindWrapper::CreateArrayOfMySqlBinds(10);
	delete[] wrapperArray;
}