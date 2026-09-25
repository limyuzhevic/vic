# NLM Test Framework CMakeLists.txt
# 
# This CMakeLists.txt provides build configuration for the NLM test framework.
# It integrates with the main NLM build system and provides proper installation
# targets for the testing framework components.

cmake_minimum_required(VERSION 3.16)

project(nlm_test_framework VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position independent code
add_library(nlm_test_framework INTERFACE)

# Include directories for the test framework
target_include_directories(nlm_test_framework INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Compiler options
target_compile_options(nlm_test_framework INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    $<$<CXX_COMPILER_ID:GNU>:-Werror>
    $<$<CXX_COMPILER_ID:Clang>:-Werror>
)

# Test framework dependencies
# The test framework depends on the main NLM library
target_link_libraries(nlm_test_framework INTERFACE nlm_core)

# Installation targets
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/base/ DESTINATION include/nlm_test_framework/base
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/types/ DESTINATION include/nlm_test_framework/types
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/utils/ DESTINATION include/nlm_test_framework/utils
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/ DESTINATION include/nlm_test_framework/benchmarks
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/main/ DESTINATION include/nlm_test_framework/main
    FILES_MATCHING PATTERN "*.hpp"
)

# Package configuration for find_package support
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_framework-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nlm_test_framework-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_framework-config.cmake
    INSTALL_DESTINATION lib/cmake/nlm_test_framework
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_framework-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_framework-config-version.cmake
    DESTINATION lib/cmake/nlm_test_framework
)

# Add tests target
if(BUILD_TESTING)
    enable_testing()
    
    # Add test executable
    add_executable(nlm_test_suite
        test_framework/main/TestFrameworkMain.cpp
    )
    
    target_link_libraries(nlm_test_suite PRIVATE nlm_test_framework nlm_core)
    target_compile_features(nlm_test_suite PRIVATE cxx_std_20)
    
    add_test(NAME nlm_test_suite COMMAND nlm_test_suite)
endif()

# Create export targets for building with find_package
export(TARGETS nlm_test_framework
    FILE ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_framework-targets.cmake
)

export(PACKAGE nlm_test_framework)

# Generate and install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nlm_test_framework.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_framework.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_framework.pc
    DESTINATION lib/pkgconfig
)

# Summary
message(STATUS "NLM Test Framework Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Test framework included: ${BUILD_TESTING}")
