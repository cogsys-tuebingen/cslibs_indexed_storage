cmake_minimum_required(VERSION 2.8.3)
project(cslibs_clustering)

include(cmake/ForceCpp11.cmake)
include(cmake/EnableWarnings.cmake)

# Boost required for boost::dynamic_bitset only :/
find_package(Boost REQUIRED)
# for tests
find_package(GTest)

# Catkin is optional
find_package(catkin QUIET)
if (${catkin_FOUND})
  catkin_package(
    INCLUDE_DIRS include
    DEPENDS boost
  )

  install(DIRECTORY include/${PROJECT_NAME}/ DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
endif ()

# Setup library
# ideally we set an interface library here but this is not supported before cmake 3.0
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDR_DIRS}
)
set(${PROJECT_NAME}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# needed for qt to recognize these files at all
file(GLOB_RECURSE include_files include/${PROJECT_NAME}/*)
add_custom_target(show_${PROJECT_NAME}_headers_in_qt SOURCES ${include_files})


add_subdirectory(example)
if (${GTEST_FOUND})
  enable_testing()
  include_directories(${GTEST_INCLUDE_DIRS})
  add_subdirectory(test)
endif()
