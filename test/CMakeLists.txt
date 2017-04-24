# needed for qt to recognize these files at all
file(GLOB_RECURSE include_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
add_custom_target(show_${PROJECT_NAME}_test_headers_in_ide SOURCES ${include_files})

# add single logical tests
create_test(test_interface_data
            interface/test_interface_data_merge.cpp
            interface/test_interface_data_storage_hint_default.cpp
            interface/test_interface_data_storage_hint_dense.cpp
            interface/test_interface_data_storage_hint_nonowning.cpp)

create_test(test_backend_integration
            backend/test_backend_integration.cpp)

create_test(test_support_stl_array interface/test_index_std_array.cpp)
create_test(test_support_stl_tuple interface/test_index_std_tuple.cpp)
create_test(test_operations_clustering_grid_neighboorhood operations/clustering/test_grid_neighborhood.cpp)
create_test(test_storage
            storage/array/*.cpp
            storage/kdtree/*.cpp)

# add "run all" target
file(GLOB_RECURSE all_test_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
build_test_exe(test_all ${all_test_sources})
