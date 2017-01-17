# needed for qt to recognize these files at all
file(GLOB_RECURSE include_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
add_custom_target(show_${PROJECT_NAME}_test_headers_in_qt SOURCES ${include_files})

# convinience macros
macro(build_test_exe name)
  file(GLOB_RECURSE sources ${ARGN})
  add_executable(test_${name} ${sources})
  target_include_directories(test_${name}
    PUBLIC
    ${GTEST_INCLUDE_DIRS}
    ${cslibs_indexed_storage_INCLUDR_DIR}
    ${Boost_INCLUDR_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )
  target_link_libraries(test_${name} PUBLIC ${GTEST_BOTH_LIBRARIES} pthread)
endmacro()

macro(create_test name)
  if (${catkin_FOUND})
    catkin_add_gtest(test_${name} ${ARGN})
  else()
    build_test_exe(${name} ${ARGN})
    add_test(test_${name} test_${name})
  endif()
endmacro()

# add single logical tests
create_test(interface_data
  interface/test_interface_data_merge.cpp
  interface/test_interface_data_storage_hint_default.cpp
  interface/test_interface_data_storage_hint_dense.cpp
  interface/test_interface_data_storage_hint_nonowning.cpp
  )

create_test(support_stl_array interface/test_index_std_array.cpp)
create_test(support_stl_tuple interface/test_index_std_tuple.cpp)
create_test(operations_clustering_grid_neighboorhood operations/clustering/test_grid_neighborhood.cpp)
create_test(storage
  storage/array/*.cpp
  storage/kdtree/*.cpp
  )

# add "run all" target
file(GLOB_RECURSE all_test_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
build_test_exe(all ${all_test_sources})