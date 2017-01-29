#include <mysql.h>

void mysql_print_error(MYSQL* mysql)
{
	printf("Error(%d) [%s] \"%s\"",
		mysql_errno(mysql),
		mysql_sqlstate(mysql),
		mysql_error(mysql));
}