cmake_minimum_required(VERSION 3.28)

include(GoogleTest)

find_package(GTest REQUIRED)

add_executable(
        bigint23_tests
        arithmetic_tests.cpp
        assign_and_equals_tests.cpp
        binary_tests.cpp
        byteswap.cpp
        comparison_tests.cpp
        io_tests.cpp
        main.cpp
)

target_link_libraries(
        bigint23_tests
        PRIVATE
        bigint23::bigint23
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(bigint23_tests)

set_target_properties(bigint23_tests PROPERTIES CXX_STANDARD 23)
target_compile_features(bigint23_tests PUBLIC cxx_std_23)
