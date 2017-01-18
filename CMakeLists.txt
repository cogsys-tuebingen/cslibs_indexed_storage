cmake_minimum_required(VERSION 2.8.3)
project(cslibs_indexed_storage)

include(cmake/ForceCpp11.cmake)
include(cmake/EnableWarnings.cmake)

find_package(Boost REQUIRED)              # Boost required (header only)
find_package(Doxygen)                     # for documentation

# Catkin is optional
set(CATKIN_ENABLE_TESTING ON)
set(CATKIN_SKIP_TESTING OFF)
find_package(catkin QUIET)
if (${catkin_FOUND})
  catkin_package(
    INCLUDE_DIRS include
    DEPENDS Boost
  )

  install(DIRECTORY include/${PROJECT_NAME}/ DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
else()
  # only include tests if we are not running under catkin which alreay provides googletest
  include(dep/googletest/googletest.cmake)  # for tests
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

if (${DOXYGEN_FOUND})
  add_subdirectory(doc)
endif()

add_subdirectory(example)
if (${GTEST_FOUND})
  enable_testing()
  add_subdirectory(test)
endif()
