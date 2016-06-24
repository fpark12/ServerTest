SET( DEFINE
	ASIO_STANDALONE
)
SET( INCLUDE
	asio
)
SET( LINK
	asio
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
