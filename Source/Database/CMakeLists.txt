SET( DEFINE
)
SET( INCLUDE
	mysql
)
SET( LINK
	3rdParty/mysql/lib/libmysql.lib
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
