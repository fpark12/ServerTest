SET( DEFINE
)
SET( INCLUDE
testdll
)
SET( LINK
testdll
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
