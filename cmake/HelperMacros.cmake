macro(create_example name)
    add_executable(${name} ${ARGN})
    target_include_directories(${name}
                               PUBLIC
                               ${cslibs_indexed_storage_INCLUDR_DIR}
                               ${Boost_INCLUDR_DIRS})
endmacro()

macro(build_test_exe name)
    file(GLOB_RECURSE sources ${ARGN})
    add_executable(${name} ${sources})
    target_include_directories(${name}
                               PUBLIC
                               ${GTEST_INCLUDE_DIRS}
                               ${cslibs_indexed_storage_INCLUDR_DIR}
                               ${Boost_INCLUDR_DIRS}
                               ${CMAKE_CURRENT_SOURCE_DIR}/include/
                               )
    target_link_libraries(${name} PUBLIC ${GTEST_BOTH_LIBRARIES} pthread)
endmacro()

macro(build_test_catkin name)
    file(GLOB_RECURSE sources ${ARGN})
    catkin_add_gtest(${name}
                     ${sources}
                     ${CMAKE_CURRENT_SOURCE_DIR}/ros_test_main.cpp
                     )
    target_include_directories(${name}
                               PUBLIC
                               ${cslibs_indexed_storage_INCLUDR_DIR}
                               ${Boost_INCLUDR_DIRS}
                               ${CMAKE_CURRENT_SOURCE_DIR}/include/
                               )
    target_link_libraries(${name} pthread)
endmacro()

macro(create_test name)
    if (${catkin_FOUND})
        build_test_catkin(${name} ${ARGN})
    else()
        build_test_exe(${name} ${ARGN})
        add_test(${name} ${name})
    endif()
endmacro()
