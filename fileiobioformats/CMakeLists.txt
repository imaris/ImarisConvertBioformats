file(GLOB HDRS application/*h  java/*h)
file(GLOB SRCS application/*.cxx  java/*.cxx)

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

include_directories(${PARENT_DIR})
include_directories(${PARENT_DIR}/fileiobase)

include_directories(${Boost_INCLUDE_DIRS})
get_filename_component(Boost_INCLUDE_PARENT_DIR ${Boost_INCLUDE_DIRS} DIRECTORY)
include_directories(${Boost_INCLUDE_PARENT_DIR})


set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
message("JNI_LIBRARIES: ${JNI_LIBRARIES}")
message("JNI_INCLUDE_DIRS: ${JNI_INCLUDE_DIRS}")
include_directories(${JNI_INCLUDE_DIRS})

set(JAR_DIR ${PARENT_DIR}/bioformats)
include_directories(${JAR_DIR})

set(tgt bpfileiobioformats)
add_library(${tgt} SHARED ${SRCS} ${HDRS})
target_include_directories(${tgt} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(${tgt} PUBLIC COMPILE_SHARED_LIBRARY)
target_link_libraries(${tgt} ${JNI_LIBRARIES} fileiobase ${Boost_LIBRARIES})


add_custom_command(TARGET bpfileiobioformats
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${JAR_DIR}
        ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/fileiobioformats)

if(CMAKE_SYSTEM_NAME MATCHES Windows)	        	

	add_custom_command(TARGET bpfileiobioformats POST_BUILD
    	COMMAND ${CMAKE_COMMAND} -E copy_directory
        	"${JRE_HOME}"
        	${CMAKE_BINARY_DIR}/${BUILD_TYPE}/jre)
else()

	add_custom_command(TARGET bpfileiobioformats  POST_BUILD
    	COMMAND ${CMAKE_COMMAND} -E create_symlink
    		"${JRE_HOME}"
    	${CMAKE_BINARY_DIR}/${BUILD_TYPE}/jre)
endif()


install (TARGETS bpfileiobioformats
	 ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}
	 LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}
	 RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE})