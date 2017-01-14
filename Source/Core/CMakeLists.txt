SET( DEFINE
)
SET( INCLUDE
utf8cpp
fmtlib
spdlog
rapidjson
libprotobuf
GUI
)
SET( LINK
utf8cpp
fmtlib
spdlog
rapidjson
libprotobuf
GUI
)

create_project(SHARED "${DEFINE}" "${INCLUDE}" "${LINK}")
