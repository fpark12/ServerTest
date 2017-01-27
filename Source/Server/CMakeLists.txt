SET( DEFINE
)
SET( INCLUDE
Core
Database
asio
gsoap
libprotobuf
)
SET( LINK
Core
Database
asio
gsoap
libprotobuf
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
