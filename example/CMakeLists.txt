macro(add_example name)
  add_executable(${name} ${ARGN})
  target_include_directories(${name}
    PUBLIC
    ${cslibs_clustering_INCLUDR_DIR}
    ${Boost_INCLUDR_DIRS}
    )
endmacro()

add_example(example_benchmark benchmark/benchmark.cpp benchmark/testdata.hpp benchmark/timing.hpp)
add_example(example_auto_index auto_index.cpp)
add_example(example_clustering clustering.cpp)
add_example(example_lookup_storage lookup_storage.cpp)
add_example(example_test test.cpp)
