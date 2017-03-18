SET( DEFINE
)
SET( INCLUDE
libmariadb
hinnantdate
)
SET( LINK
libmariadb
hinnantdate
)
create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
