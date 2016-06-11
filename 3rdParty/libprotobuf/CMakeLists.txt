SET( DEFINE
)
SET( INCLUDE
)
SET( LINK
)
add_definitions(/wd4244 /wd4267 /wd4018 /wd4355 /wd4800 /wd4251 /wd4996 /wd4146 /wd4305)
create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
