# Copyright 2018 Yeolar

set(FLATTYPE_BASE_TEST_SRCS
    SerializeTest.cpp
    StringizeTest.cpp
    TypeTest.cpp
    ValueTest.cpp
)

foreach(test_src ${FLATTYPE_BASE_TEST_SRCS})
    get_filename_component(test_name ${test_src} NAME_WE)
    set(test flattype_base_${test_name})
    add_executable(${test} ${test_src})
    target_link_libraries(${test} ${GTEST_BOTH_LIBRARIES} flattype_static)
    add_test(${test} ${test} CONFIGURATIONS ${CMAKE_BUILD_TYPE})
endforeach()

