SET( DEFINE
)
SET( INCLUDE
3rdparty/boost/preprocessor
libmariadb
hinnantdate
)
SET( LINK
libmariadb
hinnantdate
)
create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
