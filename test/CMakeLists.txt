# needed for qt to recognize these files at all
file(GLOB_RECURSE include_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
add_custom_target(show_${PROJECT_NAME}_test_headers_in_qt SOURCES ${include_files})

macro(create_test name)
  add_executable(${name} ${ARGN})
  target_include_directories(${name}
    PUBLIC
      ${GTEST_INCLUDE_DIRS}
      ${cslibs_clustering_INCLUDR_DIR}
      ${Boost_INCLUDR_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )
  target_link_libraries(${name} PUBLIC ${GTEST_BOTH_LIBRARIES} pthread)
  add_test(${name}_test ${name})
endmacro()

create_test(support_stl_array support/test_support_stl_array.cpp)
create_test(operations_clustering_grid_neighboorhood operations/clustering/test_grid_neighborhood.cpp)
