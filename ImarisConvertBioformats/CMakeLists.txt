cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(ImarisConvertBioformats)

set (CMAKE_SYSTEM_VERSION 8.1 CACHE TYPE INTERNAL FORCE)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12.0") 
    cmake_policy(SET CMP0074 NEW)
endif()

cmake_policy(SET CMP0060 NEW)

set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_STANDARD 14)
enable_language(CXX)


if(CMAKE_SYSTEM_NAME MATCHES Windows)
    set(CMAKE_CXX_FLAGS "-Zm200 /EHsc /bigobj -w44101 -w44244 -w44267 -w34062 -w34263 -w34265 -w34287 -w34289 -w34296 -w34431 -w34057 -w34092 -w34131 -w34132 -w34189 -w34202 -w34208 -w34245 -w34268 -w34295 -w34389 -w34456 -w34457 -w34505 -w34515 -w34516" CACHE INTERNAL "")
	add_definitions(-D_WINDOWS -DWIN32 -D_ENABLE_EXTENDED_ALIGNED_STORAGE -DWIN64 -DIMARIS_VERSION_MAJOR=9 -DIMARIS_VERSION_MINOR=9 -DIMARIS_VERSION_PATCH=0 -DIMARISCONVERT_VERSION_MAJOR=9 -DIMARISCONVERT_VERSION_MINOR=9 -DIMARISCONVERT_VERSION_PATCH=0 -DNOMINMAX -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_WIN32_WINNT=0x0601 -DQT_NO_DEBUG -DNDEBUG)
elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
	set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -fPIC -pedantic -W -Wwrite-strings \
  -Wcast-align -Wcast-qual -Wpointer-arith -Wshadow -Wendif-labels \
  -Wundef -Wfloat-equal -Wreorder -Wconversion \
    -g -g3 -ggdb3 -Wno-sign-conversion -Wno-conversion \
  -Wno-float-equal -Wno-unused-parameter -O2 -std=gnu++1y -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk" CACHE INTERNAL "")
	add_definitions(-D__APPLE__ -DIMARIS_VERSION_MAJOR=9 -DIMARIS_VERSION_MINOR=9 -DIMARIS_VERSION_PATCH=0 -DIMARISCONVERT_VERSION_MAJOR=9 -DIMARISCONVERT_VERSION_MINOR=9 -DIMARISCONVERT_VERSION_PATCH=0)
else()
	set(CMAKE_CXX_FLAGS "-std=c++14 -O2 -fPIC -W -fpermissive -Wwrite-strings -Wno-sign-conversion -Wno-conversion -Wno-unused-parameter\
	-Wcast-align -Wcast-qual -Wpointer-arith -Wshadow -Wendif-labels -Wundef -Wno-reorder" CACHE INTERNAL "")
	add_definitions(-D__linux__ -DIMARIS_VERSION_MAJOR=9 -DIMARIS_VERSION_MINOR=9 -DIMARIS_VERSION_PATCH=0 -DIMARISCONVERT_VERSION_MAJOR=9 -DIMARISCONVERT_VERSION_MINOR=9 -DIMARISCONVERT_VERSION_PATCH=0)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -DDEBUG -D_DEBUG")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_definitions(-D_SBCS -D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_HDF5USEDLL_ -DH5_BUILT_AS_DYNAMIC_LIB)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MACOSX_RPATH 1)

find_package(HDF5 REQUIRED COMPONENTS C CXX)
if(HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIR})
    set(_hdf5_libs ${HDF5_LIBRARIES})
    message("Found HDF." + ${HDF5_INCLUDE_DIR} + " libs " + ${HDF5_LIBRARIES})
else()
    message("Not found and not installing HDF.")
endif()

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
    message("Found ZLIB." + ${ZLIB_INCLUDE_DIR} + "  " + ${ZLIB_LIBRARY})
else()
    message("Not found and not installing ZLIB.")
endif()


if(CMAKE_SYSTEM_NAME MATCHES Linux)
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)

	add_definitions( -DBOOST_ALL_NO_LIB )
	add_definitions( -DBOOST_ALL_DYN_LINK )
else()
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
endif()

find_package(Boost 1.65 REQUIRED COMPONENTS thread filesystem)
include_directories(${Boost_INCLUDE_DIRS})
get_filename_component(Boost_INCLUDE_PARENT_DIR ${Boost_INCLUDE_DIRS} DIRECTORY)
include_directories(${Boost_INCLUDE_PARENT_DIR})
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")


if(CMAKE_SYSTEM_NAME MATCHES Windows)
	set(BUILD_TYPE $<CONFIGURATION>)
else()
	set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
	if (CMAKE_BUILD_TYPE)
		add_custom_target(build-time-make-directory ALL
			COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_TYPE})
	endif()

endif()


get_filename_component(SUBDIR_LIBS ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
add_subdirectory(${SUBDIR_LIBS}/fileiobase ${SUBDIR_LIBS}/fileiobase/build)
add_subdirectory(${SUBDIR_LIBS}/ImarisWriter ${SUBDIR_LIBS}/ImarisWriter/build)
add_subdirectory(${SUBDIR_LIBS}/fileiobioformats ${SUBDIR_LIBS}/fileiobioformats/build)


set (IMARIS_WRITER_DIR ${SUBDIR_LIBS}/ImarisWriter/writer)
file(GLOB WRITER_HDRS ${IMARIS_WRITER_DIR}/bpDeriche.h ${IMARIS_WRITER_DIR}/bpImsUtils.h)
file(GLOB WRITER_SRCS ${IMARIS_WRITER_DIR}/bpDeriche.cxx ${IMARIS_WRITER_DIR}/bpImsUtils.cxx)
file(GLOB HDRS main/*h  meta/*h src/*h thumbnailFile/*h ${WRITER_HDRS})
file(GLOB SRCS main/*cxx  meta/*cxx src/*cxx thumbnailFile/*cxx ${WRITER_SRCS})

include_directories(${SUBDIR_LIBS})
include_directories(${SUBDIR_LIBS}/ImarisWriter)
include_directories(${SUBDIR_LIBS}/fileiobase)
include_directories(${SUBDIR_LIBS}/fileiobioformats)


#set(FreeImage_ROOT $(FreeImage_ROOT) CACHE PATH "Search path for the FreeImage libraries")
find_library(FreeImage_LIBRARIES 
                 NAMES FreeImage libfreeimage
                 PATHS ${FreeImage_ROOT}/lib
                 NO_DEFAULT_PATH)

get_filename_component(FreeImage_LIBRARY_DIRS ${FreeImage_LIBRARIES} DIRECTORY)
find_path(FreeImage_INCLUDE_DIR NAMES FreeImage.h
        PATHS ${FreeImage_ROOT}/include)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FreeImage DEFAULT_MSG FreeImage_INCLUDE_DIR FreeImage_LIBRARIES)
message("Found FreeImage." + ${FreeImage_INCLUDE_DIR} + "  " + ${FreeImage_LIBRARIES})
include_directories(${FreeImage_INCLUDE_DIR})


set(tgt ImarisConvertBioformats)
add_executable(${tgt} ${SRCS} ${HDRS})
target_include_directories(${tgt} PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${SUBDIR_LIBS} ${SUBDIR_LIBS}/ImarisWriter)
target_link_libraries(${tgt} PRIVATE bpfileiobioformats fileiobase  bpImarisWriter96 ${_hdf5_libs} ${ZLIB_LIBRARY} ${FreeImage_LIBRARIES} ${Boost_LIBRARIES})


get_filename_component(ZLIB_DIR ${ZLIB_INCLUDE_DIR} DIRECTORY)

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	add_custom_command(TARGET ImarisConvertBioformats POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${HDF5_ROOT}/bin
			${CMAKE_BINARY_DIR}/${BUILD_TYPE})

	add_custom_command(TARGET ImarisConvertBioformats POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${FreeImage_ROOT}/bin
				${CMAKE_BINARY_DIR}/${BUILD_TYPE})

	add_custom_command(TARGET ImarisConvertBioformats POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
				${ZLIB_DIR}/bin
				${CMAKE_BINARY_DIR}/${BUILD_TYPE})
else()
	if (CMAKE_BUILD_TYPE)
		install (TARGETS ImarisConvertBioformats
		 ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}
		 LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}
		 RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE})
	endif()
endif()


install (TARGETS bpImarisWriter96
	 ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}
	 LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}
	 RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE})
