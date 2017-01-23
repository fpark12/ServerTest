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

create_project(SHARED "${DEFINE}" "${INCLUDE}" "${LINK}")
