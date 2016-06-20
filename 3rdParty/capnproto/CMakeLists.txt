SET( DEFINE
	CAPNP_LITE
)
SET( INCLUDE
)
SET( LINK
)
add_definitions(/wd4244 /wd4267 /wd4018 /wd4355 /wd4800 /wd4251 /wd4996 /wd4146 /wd4305)
create_project(STATIC "${DEFINE}" "${INCLUDE}" "${LINK}")

if(TARGET ${PROJECT_NAME})
	#message("Running ${CMAKE_CURRENT_SOURCE_DIR}/CMakelists.txt")
	
	file(GLOB_RECURSE protoFiles ${CMAKE_SOURCE_DIR}/*.capnp)
	file(GLOB_RECURSE CapnProto ${CMAKE_SOURCE_DIR}/capnp.exe)
	file(GLOB_RECURSE CapnProtoCPP ${CMAKE_SOURCE_DIR}/capnpc-c++.exe)
	file(GLOB_RECURSE ProtoCompiler ${CMAKE_SOURCE_DIR}/protoc.exe)
	file(GLOB_RECURSE existingProtoSourceList ${${PROJECT_NAME}_BINARY_DIR}/*.capnp.cpp ${${PROJECT_NAME}_BINARY_DIR}/*.capnp.h)
	#build new proto stamp
	set(newProtoStamp "")
	foreach(newFile ${protoFiles})
		file(TIMESTAMP ${newFile} newStamp)
		list(APPEND newProtoStamp "${newFile}, ${newStamp}")
		#message("newfile: ${newFile}, ${newStamp}")
	endforeach()

	#message("CapnProto: ${CapnProto}")
	#message("CapnProtoCPP: ${CapnProtoCPP}")
	#write new stamp to file
	file(WRITE ${${PROJECT_NAME}_BINARY_DIR}/proto.stamp "${newProtoStamp}")

	set(newProtoSourceList "")
	foreach(PROTO_FILE ${protoFiles})
		get_filename_component(PROTO_FILE_DIR ${PROTO_FILE} DIRECTORY)
		get_filename_component(PROTO_FILE_NAME ${PROTO_FILE} NAME_WE)

		execute_process(
			COMMAND ${CapnProto}
			compile
			--src-prefix=${PROTO_FILE_DIR}
			-o${CapnProtoCPP}:${${PROJECT_NAME}_BINARY_DIR}
			${PROTO_FILE}
		)

		file(GLOB_RECURSE CapnpCPPFile ${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.c++)
		if(CapnpCPPFile)
			file(RENAME
				${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.c++
				${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.cpp)
			
			#message("${PROJECT_NAME}: ${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.pb.cc")
			#message("${PROJECT_NAME}: ${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.pb.h")
			target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.cpp)
			target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.h)
			SOURCE_GROUP("ProtoGenerated" FILES ${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.cpp)
			SOURCE_GROUP("ProtoGenerated" FILES ${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.h)
			list(APPEND newProtoSourceList "${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.cpp")
			list(APPEND newProtoSourceList "${${PROJECT_NAME}_BINARY_DIR}/${PROTO_FILE_NAME}.capnp.h")
		endif()
	endforeach()

	#delete useless proto source files
	#message("existingProtoSourceList: ${existingProtoSourceList}")
	#message("newProtoSourceList: ${newProtoSourceList}")
	foreach(newProtoSource ${newProtoSourceList})
		list(REMOVE_ITEM existingProtoSourceList "${newProtoSource}")
	endforeach()
	#message("should delete: ${existingProtoSourceList}")
	if(existingProtoSourceList)
		file(REMOVE ${existingProtoSourceList})
	endif()
	add_custom_target(
		${PROJECT_NAME}_UpdateProto
		DEPENDS always_rebuild
	)

	add_custom_command(
		OUTPUT always_rebuild
		COMMAND ${CMAKE_COMMAND} -E echo
	)

	add_custom_command(
		VERBATIM
		TARGET ${PROJECT_NAME}_UpdateProto
		PRE_BUILD
		COMMAND
			${CMAKE_COMMAND}
			-DbuildDir=${CMAKE_BINARY_DIR}
			-DprojBuildDir=${${PROJECT_NAME}_BINARY_DIR}
			-DprojDir=${CMAKE_CURRENT_SOURCE_DIR}
			-DslnDir=${CMAKE_SOURCE_DIR}
			-Dgenerator=${CMAKE_GENERATOR}
			-P ${CMAKE_CURRENT_SOURCE_DIR}/UpdateProto.cmake
		#COMMAND ${CMAKE_COMMAND} -E touch "D:/Github/ServerTest/Build/3rdParty/libprotobuf/CMakeFiles/generate.stamp"
		#COMMAND ${CMAKE_COMMAND} -E touch "D:/Github/ServerTest/Build/CMakeFiles/generate.stamp"
		COMMENT "Checking Proto Files..."
		#COMMAND ${CMAKE_COMMAND} -E echo "ZERO_CHECKZERO_CHECKZERO_CHECKZERO_CHECKZERO_CHECKZERO_CHECKZERO_CHECKZERO_CHECKZERO_CHECKZERO_CHECK "
	)	

	add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_UpdateProto)

	if( MSVC )
		SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
		SET_PROPERTY(TARGET ${PROJECT_NAME}_UpdateProto		PROPERTY FOLDER CMakePredefinedTargets)
	endif()

endif()                                                     