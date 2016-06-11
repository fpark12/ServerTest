SET( DEFINE
)
SET( INCLUDE
	${CMAKE_SOURCE_DIR}///./3rdParty/mysql/include
	C:/boost_1_59_0
)
SET( LINK
	"C:/Program Files/MySQL/MySQL Server 5.7/lib/libmysql.lib"
)

create_project(CONSOLE "${DEFINE}" "${INCLUDE}" "${LINK}")
