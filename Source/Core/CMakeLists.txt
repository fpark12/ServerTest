SET( DEFINE
)
SET( INCLUDE
fmtlib
spdlog
rapidjson
libprotobuf
GUI
)
SET( LINK
fmtlib
spdlog
rapidjson
libprotobuf
GUI
)

create_project(SHARED "${DEFINE}" "${INCLUDE}" "${LINK}")
