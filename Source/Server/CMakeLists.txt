SET( DEFINE
)
SET( INCLUDE
Core
asio
)
SET( LINK
Core
asio
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
