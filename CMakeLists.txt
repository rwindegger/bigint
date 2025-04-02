cmake_minimum_required(VERSION 3.28)

project(bigint23 LANGUAGES C CXX VERSION 2.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

add_library(
        bigint23
        INTERFACE
        include/bigint23/bigint23.h
)
add_library(bigint23::bigint23 ALIAS bigint23)

target_compile_features(bigint23 INTERFACE cxx_std_23)

target_include_directories(
        bigint23
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
        TARGETS bigint23
        EXPORT bigint23Targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

set(bigint23_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/bigint23")

install(
        EXPORT bigint23Targets
        FILE bigint23Targets.cmake
        NAMESPACE bigint23::
        DESTINATION ${bigint23_CMAKE_CONFIG_DESTINATION}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/bigint23ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/bigint23Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/bigint23Config.cmake
        INSTALL_DESTINATION ${bigint23_CMAKE_CONFIG_DESTINATION}
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bigint23ConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/bigint23Config.cmake
        DESTINATION ${bigint23_CMAKE_CONFIG_DESTINATION}
)

export(
        EXPORT bigint23Targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/bigint23Targets.cmake
        NAMESPACE bigint23::
)
export(PACKAGE bigint23)

option(BUILD_TESTING "..." ON)
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
