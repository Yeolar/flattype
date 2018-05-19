# Copyright 2018 Yeolar

set(FLATTYPE_OBJECT_TEST_SRCS
    DynamicTest.cpp
)

foreach(test_src ${FLATTYPE_OBJECT_TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    set(test flattype_object_${test_name})
    add_executable(${test} ${test_src})
    target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} flattype_static)
    add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
endforeach()

