SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
	capnproto
	mysql
)
SET( LINK
	3rdParty/mysql/lib/libmysql.lib
	capnproto
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
