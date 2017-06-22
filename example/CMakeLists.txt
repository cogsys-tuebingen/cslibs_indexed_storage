create_example(example_benchmark
               benchmark/benchmark.cpp
               benchmark/testdata.hpp
               benchmark/timing.hpp)

create_example(example_auto_index
               auto_index.cpp)

create_example(example_clustering
               clustering.cpp)

create_example(example_non_owning_storage
               non_owning_storage.cpp)

create_example(example_test
               test.cpp)

create_example(example_array
               array.cpp)
create_example(example_eigen
        eigen.cpp)
target_compile_options(example_eigen PUBLIC -march=native)
