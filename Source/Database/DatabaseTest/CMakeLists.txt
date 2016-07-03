SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
	Database
)
SET( LINK
	Database
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
