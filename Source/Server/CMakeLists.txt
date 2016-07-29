SET( DEFINE
)
SET( INCLUDE
Core
asio
gsoap
)
SET( LINK
Core
asio
gsoap
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
