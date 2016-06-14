SET( DEFINE
)
SET( INCLUDE
	mysql
	libprotobuf
)
SET( LINK
	3rdParty/mysql/lib/libmysql.lib
	libprotobuf
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
