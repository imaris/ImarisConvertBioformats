file(GLOB HDRS application/*h  exceptions/*h readers/*h  types/*h  utils/*h)
file(GLOB SRCS application/*.cxx  exceptions/*.cxx readers/*.cxx  types/*.cxx  utils/*.cxx)

include_directories(${Boost_INCLUDE_DIRS})
get_filename_component(Boost_INCLUDE_PARENT_DIR ${Boost_INCLUDE_DIRS} DIRECTORY)
include_directories(${Boost_INCLUDE_PARENT_DIR})

get_filename_component(INCLUDE_PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
include_directories(${INCLUDE_PARENT_DIR})


set(tgt fileiobase)
add_library(${tgt} STATIC ${SRCS} ${HDRS})
target_include_directories(${tgt} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${tgt} ${Boost_LIBRARIES})


install (TARGETS fileiobase
	 ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}
	 LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE})    

