SET( DEFINE
)
SET( INCLUDE
openssl
zlib
)
SET( LINK
openssl
zlib
)

IF(NOT WIN32)
  INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindIconv.cmake)
  set( LINK ${LINK} ${ICONV_LIBRARIES})
ENDIF()

create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")

message("${ICONV_LIBRARIES}")

IF(WIN32)
  SET(SYSTEM_LIBS ws2_32 advapi32 kernel32)
ELSE()
  SET(SYSTEM_LIBS ${LIBPTHREAD} ${LIBDL} ${LIBM})
  IF(ICONV_EXTERNAL)
    SET(SYSTEM_LIBS ${SYSTEM_LIBS} ${ICONV_LIBRARIES})
  ENDIF()
ENDIF()
IF(WITH_SSL)
  SET(SYSTEM_LIBS ${SYSTEM_LIBS} ${SSL_LIBRARIES})
ENDIF()
MARK_AS_ADVANCED(SYSTEM_LIBS)

IF(UNIX)
  #SEARCH_LIBRARY(LIBDL dlopen "dl")
  #SEARCH_LIBRARY(LIBM floor m)
  #SEARCH_LIBRARY(LIBPTHREAD pthread_getspecific "pthread;pthreads")
  #SEARCH_LIBRARY(LIBNSL gethostbyname_r "nsl_r;nsl")
  #SEARCH_LIBRARY(LIBSOCKET setsockopt socket)
  FIND_PACKAGE(Threads)
  SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${LIBNSL} ${LIBBIND} ${LIBICONV}
    ${LIBSOCKET} ${LIBDL} ${LIBM} ${LIBPTHREAD})
  SET(SYSTEM_LIBS ${LIBNSL} ${LIBBIND} ${LIBICONV}
    ${LIBSOCKET} ${LIBDL} ${LIBM} ${LIBPTHREAD})
  MESSAGE(STATUS "SYSTEM_LIBS ${SYSTEM_LIBS}")
  #remove possible dups from required libraries
  LIST(LENGTH CMAKE_REQUIRED_LIBRARIES rllength)
  IF(${rllength} GREATER 0)
    LIST(REMOVE_DUPLICATES CMAKE_REQUIRED_LIBRARIES)
  ENDIF()
ENDIF()

if(TARGET ${PROJET_NAME})
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SYSTEM_LIBS})
endif()
if(MSVC)
	add_definitions(/wd4996)
endif()

SET(PROTOCOL_VERSION 10) # we adapted new password option from PHP's mysqlnd !
SET(MARIADB_CLIENT_VERSION_MAJOR "10")
SET(MARIADB_CLIENT_VERSION_MINOR "1")
SET(MARIADB_CLIENT_VERSION_PATCH "7")
SET(MARIADB_CLIENT_VERSION "${MARIADB_CLIENT_VERSION_MAJOR}.${MARIADB_CLIENT_VERSION_MINOR}.${MARIADB_CLIENT_VERSION_PATCH}")
MATH(EXPR MARIADB_VERSION_ID "${MARIADB_CLIENT_VERSION_MAJOR} * 10000 +
                              ${MARIADB_CLIENT_VERSION_MINOR} * 100   +
                              ${MARIADB_CLIENT_VERSION_PATCH}")
IF (NOT MARIADB_PORT)
  SET(MARIADB_PORT 3306)
ENDIF ()
IF(NOT MARIADB_UNIX_ADDR)
  SET(MARIADB_UNIX_ADDR "/tmp/mysql.sock")
ENDIF()

# todo: we don't character sets in share - all is compiled in
SET(SHAREDIR "share")
SET(DEFAULT_CHARSET_HOME "${CMAKE_INSTALL_PREFIX}")


# check for various include files
#####INCLUDE(${PROJECT_SOURCE_DIR}/cmake/CheckIncludeFiles.cmake)
# check for various functions
#####INCLUDE(${PROJECT_SOURCE_DIR}/cmake/CheckFunctions.cmake)
# check for various types
#####INCLUDE(${${PROJECT_NAME}_SOURCE_DIR}/cmake/CheckTypes.cmake)

if(TARGET ${PROJECT_NAME})
	MACRO(REGISTER_PLUGIN name source struct type target allow)
	  SET(PLUGIN_TYPE ${${name}})
	  IF(NOT PLUGIN_TYPE STREQUAL "OFF" AND NOT PLUGIN_TYPE)
	    SET(PLUGIN_TYPE ${type})
	  ENDIF()
	  IF(PLUGINS)
	    LIST(REMOVE_ITEM PLUGINS ${name})
	  ENDIF()
	  SET(${name}_PLUGIN_SOURCE ${source})
	  MARK_AS_ADVANCED(${name}_PLUGIN_SOURCE})
	  SET(${name}_PLUGIN_TYPE ${PLUGIN_TYPE})
	  IF(NOT ${target} STREQUAL "")
	    SET(${name}_PLUGIN_TARGET ${target})
	  ENDIF()
	  SET(${name}_PLUGIN_STRUCT ${struct})
	  SET(${name}_PLUGIN_SOURCE ${source})
	  SET(${name}_PLUGIN_CHG ${allow})
	  SET(PLUGINS ${PLUGINS} "${name}")
	  ADD_DEFINITIONS(-DHAVE_${name}=1)
	ENDMACRO()

	MARK_AS_ADVANCED(PLUGINS)

	# CIO
	REGISTER_PLUGIN("SOCKET" "${PROJECT_SOURCE_DIR}/plugins/pvio/pvio_socket.c" "pvio_socket_plugin" "STATIC" pvio_socket 0)
	IF(WIN32)
	  REGISTER_PLUGIN("NPIPE" "${PROJECT_SOURCE_DIR}/plugins/pvio/pvio_npipe.c" "pvio_npipe_plugin" "STATIC" pvio_npipe 1)
	  REGISTER_PLUGIN("SHMEM" "${PROJECT_SOURCE_DIR}/plugins/pvio/pvio_shmem.c" "pvio_shmem_plugin" "STATIC" pvio_shmem 1)
	ENDIF()

	# AUTHENTICATION
	REGISTER_PLUGIN("AUTH_NATIVE" "${PROJECT_SOURCE_DIR}/plugins/auth/my_auth.c" "native_password_client_plugin" "STATIC" "" 0)
	REGISTER_PLUGIN("AUTH_OLDPASSWORD" "${PROJECT_SOURCE_DIR}/plugins/auth/old_password.c" "old_password_client_plugin" "STATIC" "" 1)
	REGISTER_PLUGIN("AUTH_DIALOG" "${PROJECT_SOURCE_DIR}/plugins/auth/dialog.c" "auth_dialog_plugin" "DYNAMIC" dialog 1)
	REGISTER_PLUGIN("AUTH_CLEARTEXT" "${PROJECT_SOURCE_DIR}/plugins/auth/mariadb_clear_text.c" "auth_cleartext_plugin" "DYNAMIC" "mysql_clear_password" 1)
	IF(WIN32)
	    SET(GSSAPI_SOURCES ${PROJECT_SOURCE_DIR}/plugins/auth/auth_gssapi_client.c ${PROJECT_SOURCE_DIR}/plugins/auth/sspi_client.c ${PROJECT_SOURCE_DIR}/plugins/auth/sspi_errmsg.c)
	    REGISTER_PLUGIN("AUTH_GSSAPI" "${GSSAPI_SOURCES}" "auth_gssapi_plugin" "DYNAMIC" "auth_gssapi_client" 1)
	ELSE()
	  IF(GSSAPI_FOUND)
	    SET(GSSAPI_SOURCES ${PROJECT_SOURCE_DIR}/plugins/auth/auth_gssapi_client.c ${PROJECT_SOURCE_DIR}/plugins/auth/gssapi_client.c ${PROJECT_SOURCE_DIR}/plugins/auth/gssapi_errmsg.c)
	    REGISTER_PLUGIN("AUTH_GSSAPI" "${GSSAPI_SOURCES}" "auth_gssapi_plugin" "DYNAMIC" "auth_gssapi_client" 1)
	  ENDIF()
	ENDIF()

	#Remote_IO
	#IF(CURL_FOUND)
	#  IF(WIN32)
	#    REGISTER_PLUGIN("REMOTEIO" "${PROJECT_SOURCE_DIR}/plugins/io/remote_io.c" "remote_io_plugin" "DYNAMIC" "remote_io" 1)
	#  ELSE()
	#    REGISTER_PLUGIN("REMOTEIO" "${PROJECT_SOURCE_DIR}/plugins/io/remote_io.c" "remote_io_plugin" "DYNAMIC" "remote_io" 1)
	#  ENDIF()
	#ENDIF()

	#Trace
	REGISTER_PLUGIN("TRACE_EXAMPLE" "${PROJECT_SOURCE_DIR}/plugins/trace/trace_example.c" "trace_example_plugin" "DYNAMIC" "trace_example" 1)

	#Connection
	REGISTER_PLUGIN("REPLICATION" "${PROJECT_SOURCE_DIR}/plugins/connection/replication.c" "connection_replication_plugin" "DYNAMIC" "replication" 1)
	REGISTER_PLUGIN("AURORA" "${PROJECT_SOURCE_DIR}/plugins/connection/aurora.c" "connection_aurora_plugin" "DYNAMIC" "aurora"  1)

	# Allow registration of additional plugins
	IF(PLUGIN_CONF_FILE)
	  INCLUDE(${PLUGIN_CONF_FILE})
	ENDIF()


	SET(LIBMARIADB_SOURCES "")

	#MESSAGE(STATUS "Plugin configuration:")
	FOREACH(PLUGIN ${PLUGINS})
	  IF(WITH_${PLUGIN}_PLUGIN AND ${${PLUGIN}_PLUGIN_CHG} GREATER 0)
	    SET(${PLUGIN}_PLUGIN_TYPE ${WITH_${PLUGIN}_PLUGIN})
	  ENDIF()
	  IF(${PLUGIN}_PLUGIN_TYPE MATCHES "STATIC")
	    SET(LIBMARIADB_SOURCES ${LIBMARIADB_SOURCES} ${${PLUGIN}_PLUGIN_SOURCE})
	    SET(EXTERNAL_PLUGINS "${EXTERNAL_PLUGINS}extern struct st_mysql_client_plugin ${${PLUGIN}_PLUGIN_STRUCT};\n")
	    SET(BUILTIN_PLUGINS "${BUILTIN_PLUGINS}(struct st_mysql_client_plugin *)&${${PLUGIN}_PLUGIN_STRUCT},\n")
	  ENDIF()
	  #MESSAGE(STATUS "${PLUGIN}: ${${PLUGIN}_PLUGIN_TYPE}")
	  MARK_AS_ADVANCED(${PLUGIN}_PLUGIN_TYPE)
	ENDFOREACH()
	#MESSAGE(STATUS "STATIC PLUGIN SOURCES: ${LIBMARIADB_SOURCES}")

	#IF(NOT REMOTEIO_PLUGIN_TYPE MATCHES "NO")
	#  FIND_PACKAGE(CURL)
	#ENDIF()

	# since some files contain multiple plugins, remove duplicates from source files 
	LIST(REMOVE_DUPLICATES LIBMARIADB_SOURCES)

	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/ma_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/ma_config.h)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/mariadb_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/mariadb_version.h)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/ma_client_plugin.c.in ${CMAKE_CURRENT_BINARY_DIR}/ma_client_plugin.c)

	MARK_AS_ADVANCED(LIBMARIADB_SOURCES)

	target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ma_client_plugin.c) 
	SOURCE_GROUP("${PROJECT_NAME}" FILES ${CMAKE_CURRENT_BINARY_DIR}/ma_client_plugin.c)
endif()

ADD_DEFINITIONS(-DSIZEOF_CHARP=${CMAKE_SIZEOF_VOID_P})
ADD_DEFINITIONS(-D ENABLED_LOCAL_INFILE)
ADD_DEFINITIONS(-D HAVE_COMPRESS)
ADD_DEFINITIONS(-D LIBMARIADB)
ADD_DEFINITIONS(-D THREAD)

IF(WIN32)
  SET(HAVE_THREADS 1)
  ADD_DEFINITIONS(-DHAVE_DLOPEN)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  IF(MSVC)
     SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996" )
  ENDIF()
ELSEIF()
  SET(HAVE_THREADS ${CMAKE_USE_PTHREADS})
ENDIF()

IF(NOT WITH_SSL AND NOT WITH_SSL STREQUAL "OFF")
  IF(WIN32)
    SET(WITH_SSL "SCHANNEL")
  ELSE()
    SET(WITH_SSL "OPENSSL")
  ENDIF()
ENDIF()


IF(NOT WITH_SSL STREQUAL "OFF")
  IF(WITH_SSL STREQUAL "OPENSSL")
    #FIND_PACKAGE(OpenSSL)
    #IF(OPENSSL_FOUND)
    #  ADD_DEFINITIONS(-DHAVE_OPENSSL -DHAVE_TLS)
    #  SET(SSL_SOURCES "${PROJECT_SOURCE_DIR}/libmariadb/secure/openssl.c")
    #  SET(SSL_LIBRARIES ${OPENSSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES})
    #  INCLUDE_DIRECTORIES(BEFORE ${OPENSSL_INCLUDE_DIR})
    #  MESSAGE(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
    #ELSE()
    #  MESSAGE(WARNING "OpenSSL not found")
    #ENDIF()
  ENDIF()
  IF(WITH_SSL STREQUAL "GNUTLS")
    FIND_PACKAGE(GnuTLS)
    IF(GNUTLS_FOUND)
      ADD_DEFINITIONS(-DHAVE_GNUTLS -DHAVE_TLS)
      SET(SSL_SOURCES "${PROJECT_SOURCE_DIR}/libmariadb/secure/gnutls.c")
      SET(SSL_LIBRARIES ${GNUTLS_LIBRARY})
    ELSE()
      MESSAGE(FATAL "GnuTLS not found")
    ENDIF()
  ENDIF()
  IF(WIN32)
    IF(WITH_SSL STREQUAL "SCHANNEL")
      ADD_DEFINITIONS(-DHAVE_SCHANNEL -DHAVE_TLS)
      SET(SSL_SOURCES "${PROJECT_SOURCE_DIR}/libmariadb/secure/schannel.c" "${PROJECT_SOURCE_DIR}/libmariadb/secure/ma_schannel.c")
      INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/plugins/pvio/")
    ENDIF()
  ENDIF()

  MARK_AS_ADVANCED(SSL_SOURCES)
ENDIF()