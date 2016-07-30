SET( DEFINE
)
SET( INCLUDE
)
SET( LINK
3rdParty/openssl/lib/libeay32MDd.lib
3rdParty/openssl/lib/ssleay32MDd.lib
)
create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
