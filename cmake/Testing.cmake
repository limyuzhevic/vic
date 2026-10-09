# CMake Testing Configuration
# cmake/Testing.cmake

cmake_minimum_required(VERSION 3.16)
project(NLM_Testing LANGUAGES CXX)

# Enable testing framework
enable_testing()

# Set up CTest
include(CTest)

# Configure testing options
option(ENABLE_UNIT_TESTS "Enable unit tests" ON)
option(ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
option(ENABLE_PERFORMANCE_TESTS "Enable performance tests" OFF)
option(ENABLE_LONG_TESTS "Enable long-running tests" OFF)

# Test directories
set(TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(TEST_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/tests")

# Unit tests configuration
if (ENABLE_UNIT_TESTS)
    add_subdirectory(${TEST_SOURCE_DIR}/unit)
endif()

# Integration tests configuration
if (ENABLE_INTEGRATION_TESTS)
    add_subdirectory(${TEST_SOURCE_DIR}/integration)
endif()

# Performance tests configuration
if (ENABLE_PERFORMANCE_TESTS)
    add_subdirectory(${TEST_SOURCE_DIR}/performance)
endif()

# Custom test commands
add_custom_target(run-all-tests
    COMMAND ctest --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run all tests"
)

add_custom_target(run-unit-tests
    COMMAND ctest --output-on-failure -R unit
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run unit tests only"
)

add_custom_target(run-integration-tests
    COMMAND ctest --output-on-failure -R integration
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run integration tests only"
)

add_custom_target(run-performance-tests
    COMMAND ctest --output-on-failure -R performance
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run performance tests only"
)

# Test coverage configuration
option(ENABLE_COVERAGE "Enable test coverage analysis" OFF)

if (ENABLE_COVERAGE)
    find_program(GCOVR_PATH gcovr PATHS /usr/local/bin /usr/bin REQUIRED)
    find_program(LCOV_PATH lcov PATHS /usr/local/bin /usr/bin REQUIRED)
    
    include(CMakeFiles/coverage.cmake OPTIONAL)
endif()

# Test reporting
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_report.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_report.cmake
    @ONLY
)

include(${CMAKE_CURRENT_BINARY_DIR}/test_report.cmake)

# Add test benchmarks
function(add_performance_benchmark benchmark_name command)
    add_custom_target(${benchmark_name}
        COMMAND ${command}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Run performance benchmark: ${benchmark_name}"
        VERBATIM
    )
endfunction()

# Common benchmarks
add_performance_benchmark(benchmark_neural_network
    "${CMAKE_CURRENT_BINARY_DIR}/benchmark --neural-network"
)

add_performance_benchmark(benchmark_plasticity
    "${CMAKE_CURRENT_BINARY_DIR}/benchmark --plasticity"
)

add_performance_benchmark(benchmark_memory
    "${CMAKE_CURRENT_BINARY_DIR}/benchmark --memory"
)

add_performance_benchmark(benchmark_integration
    "${CMAKE_CURRENT_BINARY_DIR}/nlm_phase6_demo"
)