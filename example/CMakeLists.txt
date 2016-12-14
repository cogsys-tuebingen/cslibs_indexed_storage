add_executable(example_benchmark
  benchmark/benchmark.cpp
  benchmark/testdata.hpp
  benchmark/timing.hpp
)
target_include_directories(example_benchmark
  PUBLIC ${cslibs_clustering_INCLUDE_DIR}
)
target_compile_options(example_benchmark
  PUBLIC -march=native)