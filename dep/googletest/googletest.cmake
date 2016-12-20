# Download and unpack googletest at configure time
configure_file(${CMAKE_SOURCE_DIR}/dep/googletest/CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(
  ${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build)

set(GTEST_FOUND TRUE)
set(GTEST_INCLUDE_DIRS ${gtest_SOURCE_DIR}/include/)
set(GTEST_LIBRARIES gtest)
set(GTEST_MAIN_LIBRARIES gtest_main)
set(GTEST_BOTH_LIBRARIES gtest gtest_main)