SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
	mysql
	capnproto
	Database
)
SET( LINK
	Database
	3rdParty/mysql/lib/libmysql.lib
	capnproto
	
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
