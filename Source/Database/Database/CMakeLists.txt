SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
	Core
	mysql
	libprotobuf
)
SET( LINK
	Core
	mysql
	libprotobuf
)

create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
