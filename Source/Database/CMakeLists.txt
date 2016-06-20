SET( DEFINE
)
SET( INCLUDE
	Common
	mysql
	capnproto
)
SET( LINK
	3rdParty/mysql/lib/libmysql.lib
	capnproto
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
