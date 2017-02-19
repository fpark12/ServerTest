SET( DEFINE
)
SET( INCLUDE
asio
utf8cpp
fmtlib
spdlog
rapidjson
libprotobuf
GUI
openssl
)
SET( LINK
asio
utf8cpp
fmtlib
spdlog
rapidjson
libprotobuf
GUI
openssl
)

create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")
