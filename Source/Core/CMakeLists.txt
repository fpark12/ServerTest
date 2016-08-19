SET( DEFINE
)
SET( INCLUDE
fmtlib
spdlog
)
SET( LINK
fmtlib
spdlog
)

create_project(SHARED "${DEFINE}" "${INCLUDE}" "${LINK}")
