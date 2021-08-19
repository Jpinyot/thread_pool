#########################
#  Thread pool library  #
#########################
## Project configuration
cmake_minimum_required(VERSION 3.10)
include_guard(GLOBAL)
cmake_policy(SET CMP0048 NEW)

project(thread_pool VERSION 1.0.0)

set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message("\n## Configuring ${PROJECT_NAME} v${PROJECT_VERSION}")

## Add compile commands
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

#####################  VERSION DEFINITION  #############################
add_definitions(-DPARSER_NISX_LIB_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DPARSER_NISX_LIB_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DPARSER_NISX_LIB_VERSION_BUILD=${PROJECT_VERSION_PATCH})
add_definitions(-DPARSER_NISX_LIB_VERSION="${PROJECT_VERSION}")
add_definitions(-DPARSER_NISX_LIBRARY) #Legacy define

##################  LOADING MODULES  ###########################
# include(${PROJECT_SOURCE_DIR}/cmake/dependencies.cmake)

####################  SOURCES AND HEADERS  #############################

option(BUILD_SHARED_LIBS "Build Shared librarys" ON)
option(BUILD_STATIC_LIBS "Build Static librarys" ON)

set(SOURCES)
set(HEADERS)

# if (${ENABLE_TESTS})
# 	set(SRC_TEST)
# 	set(HDRS_TEST)
# endif()

add_definitions(-DTIXML_USE_STL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
	# ${LIB_PREFIX}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include
	)


add_subdirectory(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include)

set (SOURCES ${SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/src
	)

set(HEADERS ${HEADERS}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	)

if (${ENABLE_TESTS})
    set(SRC_TEST ${SRC_TEST})
	set(HDRS_TEST ${HDRS_TEST})
endif()

##############################
##           Targets        ##
##############################

#Create a virtual target to avoid multiple compilation of sources
add_library(${PROJECT_NAME}_lib OBJECT ${SOURCES} ${HEADERS})

if (${BUILD_STATIC_LIBS})
	add_library(${PROJECT_NAME}-static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_lib>)
	set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME}-static ${EXTERNAL_LIBS} ${STATIC_DEPENDENCIES})
	install(TARGETS ${PROJECT_NAME}-static LIBRARY DESTINATION ${LIB_PREFIX}/lib)
endif()

if (${BUILD_SHARED_LIBS})
	add_library(${PROJECT_NAME} SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_lib>)
	set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
	target_link_directories(${PROJECT_NAME} PUBLIC ${LIB_PREFIX}/lib)
	target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS} ${SHARED_DEPENDENCIES})
	set_property(TARGET ${PROJECT_NAME}_lib PROPERTY POSITION_INDEPENDENT_CODE 1)
endif()

if (${ENABLE_TESTS})
	message("Adding ${PROJECT_NAME} tests dir ${PROJECT_BINARY_DIR}" )
	set(PROJECT_TEST_NAME ${PROJECT_NAME}Tests)
	link_directories(${PROJECT_BINARY_DIR}/lib)

	add_executable(${PROJECT_TEST_NAME} ${SRC_TEST} ${HDRS_TEST})
	target_link_libraries(${PROJECT_TEST_NAME} ${GTEST_LIBS} ${EXTERNAL_LIBS} ${PROJECT_NAME})
	add_test(${PROJECT_NAME} ${PROJECT_TEST_NAME})
endif()
