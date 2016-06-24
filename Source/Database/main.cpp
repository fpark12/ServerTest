// Include Header Files
#include <iostream>
#include <cstdio>
#include <cstdlib>

// For MySQL Connection
#include <mysql.h>
#include "Bind.h"

using namespace std;

// Defining Constant Variables
#define SERVER "localhost"
#define USER "root"
#define PASSWORD "Keathalin21"
#define DATABASE "test"

int main()
{
	
	// Assume a scenario where we want to query "select user from Users where region = ? and age = ?"
	Bind r_bind(std::vector<QueryParamSizeInfo>{ {TYPE_STRING, 20}, {TYPE_UI8, 0}});
	return 0;
}