SET( DEFINE
)
SET( INCLUDE
Core
asio
gsoap
libprotobuf
)
SET( LINK
Core
asio
gsoap
libprotobuf
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
