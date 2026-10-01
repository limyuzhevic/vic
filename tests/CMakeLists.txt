# CMake build file for NLM Phase 6 Improved Components Tests
# This file enables building and testing all the enhanced components

cmake_minimum_required(VERSION 3.16)
project(nlm_improved_components_tests LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../python
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Test source files
set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_improved_components.cpp
)

# External library dependencies
find_package(Threads REQUIRED)

# Create test executable
add_executable(nlm_test_improved_components ${TEST_SOURCES})

# Target link libraries
# Note: The actual library names may vary based on your project structure
target_link_libraries(nlm_test_improved_components
    PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
)

# Add compiler-specific definitions
if(MSVC)
    target_compile_definitions(nlm_test_improved_components PRIVATE _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
else()
    target_compile_definitions(nlm_test_improved_components PRIVATE -D_THREAD_SAFE -D_REENTRANT)
endif()

# Include directories for dependencies
target_include_directories(nlm_test_improved_components PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../python
)

# Compiler optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nlm_test_improved_components PRIVATE -O3 -march=native -flto)
    target_link_options(nlm_test_improved_components PRIVATE -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(nlm_test_improved_components PRIVATE -g -O0 -DDEBUG)
    target_link_options(nlm_test_improved_components PRIVATE -g)
endif()

# Test target properties
set_target_properties(nlm_test_improved_components PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_NAME "nlm_test_improved_components"
    DEBUG_POSTFIX "d"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Install target (optional)
install(TARGETS nlm_test_improved_components
    RUNTIME DESTINATION bin
    NAMELINK_SKIP
    EXCLUDE_FROM_ALL
)

# Add custom target for running tests
add_custom_target(run_improved_components_tests
    COMMAND nlm_test_improved_components
    DEPENDS nlm_test_improved_components
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running NLM Phase 6 Improved Components Tests"
    VERBATIM
)

# Add test target for CTest integration (if CTest is available)
if(CTEST_FOUND)
    enable_testing()
    add_test(NAME ImprovedComponentsTests
        COMMAND nlm_test_improved_components
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# Summary target for easy build
add_custom_target(build_and_run_improved_tests
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR} --target nlm_test_improved_components
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/nlm_test_improved_components
    DEPENDS nlm_test_improved_components
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building and running NLM Phase 6 Improved Components Tests"
    VERBATIM
)

# Print build summary
message(STATUS "=========================================")
message(STATUS "NLM Phase 6 Improved Components Tests")
message(STATUS "=========================================")
message(STATUS "Test source files:")
foreach(src ${TEST_SOURCES})
    message(STATUS "  ${src}")
endforeach()
message(STATUS "")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  nlm_test_improved_components - The main test executable")
message(STATUS "  run_improved_components_tests    - Target to run tests")
message(STATUS "  build_and_run_improved_tests      - Target to build and run tests")
message(STATUS "")
message(STATUS "=========================================")