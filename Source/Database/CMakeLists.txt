SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
	Common
	mysql
	capnproto
)
SET( LINK
	3rdParty/mysql/lib/libmysql.lib
	capnproto
	Common
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
