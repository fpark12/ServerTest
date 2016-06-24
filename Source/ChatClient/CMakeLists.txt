SET( DEFINE
	ASIO_STANDALONE
)
SET( INCLUDE
	Common
	asio
)
SET( LINK
	Common
	asio
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
