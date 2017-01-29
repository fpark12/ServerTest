#get all proto files
file(GLOB_RECURSE newList ${slnDir}/*.proto)

#read stamp if exists
if(EXISTS ${projBuildDir}/proto.stamp)
	file(READ ${projBuildDir}/proto.stamp oldProtoStamp)
endif()

#build new proto stamp
set(newProtoStamp "")
foreach(newFile ${newList})
	file(TIMESTAMP ${newFile} newStamp)
	list(APPEND newProtoStamp "${newFile}, ${newStamp}")
	#message("newfile: ${newFile}, ${newStamp}")
endforeach()

#write new stamp to file
file(WRITE ${projBuildDir}/proto.stamp "${newProtoStamp}")
if(NOT "${oldProtoStamp}" STREQUAL "${newProtoStamp}"
	AND NOT "${oldProtoStamp}" STREQUAL "")
	#message("NOT EQUAL!!!!!!!!!!!!!!!!!!!!!!")
	#file(REMOVE_RECURSE "${buildDir}/x64/Debug/ZERO_CHECK/")

	#message("generator : ${generator}")
	#message("-DslnDir=${slnDir}")
	file(WRITE ${slnDir}/Build/null.log "")
	#message("generator ${generator}")
	EXECUTE_PROCESS(
		COMMAND ${CMAKE_COMMAND} -G "${generator}" "${slnDir}/CMakelists.txt" "${buildDir}"
		)
	EXECUTE_PROCESS(
		COMMAND ${CMAKE_COMMAND}
			-DCMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}
			-DCMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
			-DslnDir=${slnDir}
			-P "${slnDir}/Purify/Core/RemoveAllBuild.cmake"
		)
endif()
#message("Old stamp: ${oldProtoStamp}")
#message("New stamp: ${newProtoStamp}")

#[[
file(READ "${projDir}/CMakeLists.txt" cmakeFile)
file(WRITE "${projDir}/CMakeLists.txt" "${cmakeFile} ")
file(WRITE "${buildDir}/x64/Debug/ZERO_CHECK/ZERO_CHECK.tlog/unsuccessfulbuild")
message("TouchCMake.cmake: Touching ${projDir}/CMakeLists.txt")
#]]
