cmake_minimum_required(VERSION 3.28)

project(bigint LANGUAGES C CXX VERSION 2.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

add_library(
        bigint
        INTERFACE
        include/bigint23/bigint.hpp
)
add_library(bigint::bigint ALIAS bigint)

target_compile_features(bigint INTERFACE cxx_std_23)

target_include_directories(
        bigint
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
        TARGETS bigint
        EXPORT bigintTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

set(bigint_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/bigint23")

install(
        EXPORT bigintTargets
        FILE bigintTargets.cmake
        NAMESPACE bigint::
        DESTINATION ${bigint_CMAKE_CONFIG_DESTINATION}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/bigintConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/bigintConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/bigintConfig.cmake
        INSTALL_DESTINATION ${bigint_CMAKE_CONFIG_DESTINATION}
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bigintConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/bigintConfig.cmake
        DESTINATION ${bigint_CMAKE_CONFIG_DESTINATION}
)

export(
        EXPORT bigintTargets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/bigintTargets.cmake
        NAMESPACE bigint::
)
export(PACKAGE bigint)

option(BUILD_TESTING "..." ON)
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
