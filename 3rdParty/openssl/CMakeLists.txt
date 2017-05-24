SET( DEFINE
)
SET( INCLUDE
)
SET( LINK
)
if(MSVC)
	add_definitions(/wd4244 /wd4267 /wd4331 /wd4273 /wd4311 /wd4996)
endif()

file( READ ${${PROJECT_NAME}_SOURCE_DIR}/opensslconf.h.in CONF )
string( REPLACE "#undef OPENSSL_EXPORT_VAR_AS_FUNCTION" "#cmakedefine OPENSSL_EXPORT_VAR_AS_FUNCTION" CONF "${CONF}" )

set( CONF "
#define OPENSSL_NO_GMP
#define OPENSSL_NO_JPAKE
#define OPENSSL_NO_KRB5
#define OPENSSL_NO_MD2
#define OPENSSL_NO_RC5
#define OPENSSL_NO_RFC3779
#define OPENSSL_NO_STORE
#define OPENSSL_NO_DYNAMIC_ENGINE
#define OPENSSL_NO_SCTP
#define OPENSSL_NO_EC_NISTP_64_GCC_128
${CONF}" )
file( WRITE ${${PROJECT_NAME}_BINARY_DIR}/openssl/opensslconf.h.in "${CONF}" )

## AND MINGW AND NOT CYGWIN? or? at least msvc needs OPENSSL_EXPORT_VAR_AS_FUNCTION
add_definitions( -DOPENSSL_NO_ASM )

if(XCODE)
  add_definitions( -DOPENSSL_SYSNAME_MACOSX )
endif()

if( WIN32 )
  add_definitions( -DOPENSSL_SYSNAME_WIN32 )
  add_definitions( -DWIN32_LEAN_AND_MEAN )
  # avoid conflict: ocsp.h and wincrypt.h
  add_definitions( -D_WINDLL )

  set( OPENSSL_EXPORT_VAR_AS_FUNCTION true )
endif()

configure_file ( ${${PROJECT_NAME}_SOURCE_DIR}/buildinf.h.in ${${PROJECT_NAME}_BINARY_DIR}/buildinf.h )
configure_file( ${${PROJECT_NAME}_BINARY_DIR}/openssl/opensslconf.h.in ${${PROJECT_NAME}_BINARY_DIR}/openssl/opensslconf.h )

#~2DO: set BUILDINF_DATE to `date LC_ALL=C LC_TIME=C`
#configure_file ( buildinf.h.cmake buildinf.h )

create_project(STATIC DEFINE INCLUDE LINK)
