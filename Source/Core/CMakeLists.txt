SET( DEFINE
)
SET( INCLUDE
fmtlib
spdlog
GUI
)
SET( LINK
fmtlib
spdlog
GUI
)

create_project(SHARED "${DEFINE}" "${INCLUDE}" "${LINK}")
