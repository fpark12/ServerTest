SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
	Core
	mysql
	capnproto
)
SET( LINK
	Core
	3rdParty/mysql/lib/libmysql.lib
	capnproto
	
)

create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
