SET( DEFINE
)
SET( INCLUDE
spdlog
)
SET( LINK
spdlog
)

create_project(SHARED "${DEFINE}" "${INCLUDE}" "${LINK}")
