# NLM Testing Framework - Modern CMake Configuration
# Comprehensive testing setup with multiple test runners and frameworks

cmake_minimum_required(VERSION 3.22)

# Test configuration options
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_ENABLE_UNIT_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
option(NLM_ENABLE_PERFORMANCE_TESTS "Enable performance tests" ON)
option(NLM_ENABLE_COVERAGE_TESTS "Enable coverage tests" OFF)
option(NLM_ENABLE_CTEST "Use CTest for test running" ON)
option(NLM_ENABLE_CATCH2 "Use Catch2 framework" OFF)
option(NLM_ENABLE_GOOGLETEST "Use GoogleTest framework" OFF)
option(NLM_ENABLE_PYTHON_TESTS "Enable Python tests" ON)

# Project configuration for testing
project(NLM_Tests
    VERSION 0.1.0
    LANGUAGES CXX
)

if(NOT NLM_BUILD_TESTS)
    return()
endif()

# Find required packages
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler configuration
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

# Include NLM core targets
include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)

# Import NLM core library
add_library(nlm_core_test ALIAS nlm_core)
add_library(nlm_agent_test ALIAS nlm_agent)
add_library(nlm_world_test ALIAS nlm_world)

# Test source groups
file(GLOB NLM_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_*.c
)

# Unit tests
if(NLM_ENABLE_UNIT_TESTS)
    add_executable(nlm_unit_tests
        ${NLM_TEST_SOURCES}
    )
    
    target_link_libraries(nlm_unit_tests PRIVATE nlm_core)
    target_include_directories(nlm_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    
    # Add test cases
    add_test(NAME nlm_unit_tests COMMAND nlm_unit_tests)
    set_target_properties(nlm_unit_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endif()

# Integration tests
if(NLM_ENABLE_INTEGRATION_TESTS)
    add_executable(nlm_integration_tests
        src/integration_test_main.cpp
        src/integration_tests/*.cpp
    )
    
    target_link_libraries(nlm_integration_tests PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(nlm_integration_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    
    add_test(NAME nlm_integration_tests COMMAND nlm_integration_tests)
    set_target_properties(nlm_integration_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endif()

# Performance tests
if(NLM_ENABLE_PERFORMANCE_TESTS)
    add_executable(nlm_performance_tests
        src/performance_test_main.cpp
        src/benchmarks.cpp
    )
    
    target_link_libraries(nlm_performance_tests PRIVATE nlm_core)
    target_include_directories(nlm_performance_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    
    add_test(NAME nlm_performance_tests COMMAND nlm_performance_tests)
    set_target_properties(nlm_performance_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endif()

# Coverage tests
if(NLM_ENABLE_COVERAGE_TESTS)
    add_executable(nlm_coverage_tests
        ${NLM_TEST_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/coverage_test_main.cpp
    )
    
    target_link_libraries(nlm_coverage_tests PRIVATE nlm_core)
    target_include_directories(nlm_coverage_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    
    # Add coverage compilation flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_coverage_tests PRIVATE --coverage -O0 -g)
        target_link_options(nlm_coverage_tests PRIVATE --coverage)
    endif()
    
    add_test(NAME nlm_coverage_tests COMMAND nlm_coverage_tests)
    set_target_properties(nlm_coverage_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endif()

# Python tests
if(NLM_ENABLE_PYTHON_TESTS)
    # Create a custom target for Python tests
    add_custom_target(python-tests
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python/ -v
        COMMENT "Run Python unit tests"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Add Python tests to CTest
    add_test(NAME python_tests COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python/)
endif()

# CTest configuration
if(NLM_ENABLE_CTEST)
    enable_testing()
    
    # Configure test properties
    set(CTEST_TEST_TIMEOUT 300)  # 5 minute timeout per test
    set(CTEST_PARALLEL_LEVEL 4)  # Run up to 4 tests in parallel
    
    # Add test properties
    set_tests_properties(nlm_unit_tests nlm_integration_tests nlm_performance_tests
        PROPERTIES
            TIMEOUT ${CTEST_TEST_TIMEOUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# Test reporting and output
add_custom_target(test-report
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${CMAKE_COMMAND} -E echo "Running tests with CTest..."
    DEPENDS nlm_unit_tests nlm_integration_tests
    COMMENT "Run comprehensive test suite"
)

# Add a comprehensive test target
set(TARGETS_TO_TEST
    nlm_unit_tests
    nlm_integration_tests
    nlm_performance_tests
)

if(NLM_ENABLE_COVERAGE_TESTS)
    list(APPEND TARGETS_TO_TEST nlm_coverage_tests)
endif()

add_custom_target(run-all-tests
    COMMAND ${CMAKE_CTEST_COMMAND} -j4
    COMMENT "Run all NLM tests"
    DEPENDS ${TARGETS_TO_TEST}
)

# Code quality checks for tests
add_custom_target(check-tests
    COMMAND ${Python_EXECUTABLE} -m flake8 ${CMAKE_CURRENT_SOURCE_DIR}/python/
    COMMAND ${Python_EXECUTABLE} -m mypy ${CMAKE_CURRENT_SOURCE_DIR}/python/
    COMMENT "Run code quality checks on tests"
)

# Performance benchmarking target
if(TARGET nlm_benchmark)
    add_custom_target(run-benchmarks
        COMMAND ./nlm_benchmark
        DEPENDS nlm_benchmark
        COMMENT "Run performance benchmarks"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Clean tests target
add_custom_target(clean-tests
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/test_results.xml
    COMMENT "Clean test artifacts"
)

message(STATUS "Test configuration complete")
message(STATUS "Unit tests: ${NLM_ENABLE_UNIT_TESTS}")
message(STATUS "Integration tests: ${NLM_ENABLE_INTEGRATION_TESTS}")
message(STATUS "Performance tests: ${NLM_ENABLE_PERFORMANCE_TESTS}")
message(STATUS "Coverage tests: ${NLM_ENABLE_COVERAGE_TESTS}")
message(STATUS "Python tests: ${NLM_ENABLE_PYTHON_TESTS}")