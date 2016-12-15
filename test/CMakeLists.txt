macro(create_test name)
  add_executable(${name} ${ARGN})
  target_include_directories(${name} PUBLIC ${GTEST_INCLUDE_DIRS} ${cslibs_clustering_INCLUDR_DIR} ${Boost_INCLUDR_DIRS})
  target_link_libraries(${name} PUBLIC ${GTEST_BOTH_LIBRARIES} pthread)
  add_test(${name}_test ${name})
endmacro()

create_test(support_stl_array support/test_support_stl_array.cpp)
