SET( DEFINE
)
SET( INCLUDE
)
SET( LINK
3rdParty/mysql/lib/libmysqlclient.a
)
create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
