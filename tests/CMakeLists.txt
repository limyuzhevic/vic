cmake_minimum_required(VERSION 3.19)

# Testing framework - Google Test (gtest)
# Fetch Google Test if not found
include(FetchContent)

# Configure Google Test with modern CMake practices
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

# Fetch Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable with Google Test
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE gtest_main nlm_core)

# Configure test output and formatting
if(MSVC)
    target_compile_options(nlm_test PRIVATE /W4 /permissive-)
else()
    target_compile_options(nlm_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable test coverage if requested
if(NLM_ENABLE_COVERAGE)
    target_compile_options(nlm_test PRIVATE --coverage)
    target_link_options(nlm_test PRIVATE --coverage)
endif()

# Enable sanitizers if requested
if(NLM_ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(nlm_test PRIVATE -fsanitize=address,undefined)
        target_link_options(nlm_test PRIVATE -fsanitize=address,undefined)
    endif()
endif()

# Performance test executable for benchmarking
add_executable(nlm_perf_benchmark
    src/agent/benchmark.cpp
)

target_link_libraries(nlm_perf_benchmark PRIVATE gtest_main nlm_core nlm_brain nlm_memory)

# Performance test configuration
target_compile_features(nlm_perf_benchmark PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(nlm_perf_benchmark PRIVATE /W4 /permissive-)
else()
    target_compile_options(nlm_perf_benchmark PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Integration test executable
add_executable(nlm_integration_test
    tests/integration_test.cpp
    tests/system_test.cpp
)

if(TARGET nlm_integration_test)
    target_link_libraries(nlm_integration_test PRIVATE nlm_core nlm_agent nlm_world nlm_brain)
    target_compile_features(nlm_integration_test PRIVATE cxx_std_20)
endif()

# Enable testing framework
enable_testing()

# Add tests to CTest with verbose output
add_test(NAME nlm_unit_test
    COMMAND nlm_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(nlm_unit_test PROPERTIES
    LABELS "unit;integration"
    TIMEOUT 300
    PASS_REGULAR_EXPRESSION "All tests passed"
)

# Add performance benchmark test
if(TARGET nlm_perf_benchmark)
    add_test(NAME nlm_performance_benchmark
        COMMAND nlm_perf_benchmark
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(nlm_performance_benchmark PROPERTIES
        LABELS "performance;benchmark"
        TIMEOUT 600
    )
endif()

# Add integration test
if(TARGET nlm_integration_test)
    add_test(NAME nlm_system_integration
        COMMAND nlm_integration_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(nlm_system_integration PROPERTIES
        LABELS "integration;system"
        TIMEOUT 900
    )
endif()

# Create test result summary target
add_custom_target(test_summary
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${CMAKE_CTEST_COMMAND} --output-junit test-results.xml
    COMMAND echo "Test results available in test-results.xml"
    COMMENT "Running comprehensive test suite and generating report"
    VERBATIM
)

# Create performance analysis target
if(TARGET nlm_perf_benchmark)
    add_custom_target(performance_analysis
        COMMAND ./nlm_perf_benchmark --analyze --output perf_analysis.json
        COMMENT "Analyzing performance benchmarks"
        VERBATIM
    )
endif()

# Test data management configuration
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)

# Generate test configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_config.cmake
    @ONLY
)
