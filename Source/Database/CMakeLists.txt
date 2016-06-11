SET( DEFINE
)
SET( INCLUDE
	3rdParty/mysql/include
	C:/boost_1_59_0
)
SET( LINK
	3rdParty/mysql/lib/mysqlcppconn-static.lib
	3rdParty/mysql/lib/mysqlcppconn.lib
	"C:/Program Files/MySQL/MySQL Server 5.7/lib/libmysql.lib"
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
