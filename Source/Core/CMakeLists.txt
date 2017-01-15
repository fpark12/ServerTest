SET( DEFINE
)
SET( INCLUDE
utf8cpp
fmtlib
spdlog
rapidjson
libprotobuf
GUI
openssl
)
SET( LINK
utf8cpp
fmtlib
spdlog
rapidjson
libprotobuf
GUI
openssl
)

create_project(SHARED "${DEFINE}" "${INCLUDE}" "${LINK}")
