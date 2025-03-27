cmake_minimum_required(VERSION 3.28)

include(GoogleTest)

find_package(GTest REQUIRED)

add_executable(
        bigint_tests
        arithmetic_tests.cpp
        assign_and_equals_tests.cpp
        binary_tests.cpp
        comparison_tests.cpp
        main.cpp
)

target_link_libraries(
        bigint_tests
        PRIVATE
        bigint::bigint
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(bigint_tests)

set_target_properties(bigint_tests PROPERTIES CXX_STANDARD 23)
target_compile_features(bigint_tests PUBLIC cxx_std_23)
