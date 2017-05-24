SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
	Core
	libmariadb
	libprotobuf
	sqlpp11
)
SET( LINK
	Core
	libmariadb
	libprotobuf
	sqlpp11
)

create_project(STATIC DEFINE INCLUDE LINK)
