cmake_minimum_required(VERSION 2.8.3)
project(cslibs_clustering)

# C++11 requirement
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wdouble-promotion")

add_definitions(-DCONFIGURE_WITH_STATIC_WARNINGS)

find_package(catkin QUIET)
if (${catkin_FOUND})
    catkin_package(
    #  INCLUDE_DIRS include
    #  LIBRARIES cslibs_clustering
    #  CATKIN_DEPENDS other_catkin_pkg
    #  DEPENDS system_lib
    )
endif()

find_package(Boost REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDR_DIRS}
)

file(GLOB_RECURSE include_files include/${PROJECT_NAME}/*)
add_custom_target(show_${PROJECT_NAME}_headers_in_qt SOURCES
    ${include_files}
)

add_executable(${PROJECT_NAME}_test
    src/test.cpp
    ${include_files}
)

set(${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_subdirectory(example)

if (${catkin_FOUND})
    install(DIRECTORY include/${PROJECT_NAME}/
            DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
endif()