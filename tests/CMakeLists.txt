# NLM Testing Framework - Enhanced CMake Configuration
# Comprehensive testing setup with different test types,
# coverage analysis, and benchmarking capabilities.

cmake_minimum_required(VERSION 3.16)

# Enable testing framework
include(CTest)

# Configure CTest
set(CTEST_OUTPUT_ON_FAILURE ON)
set(CTEST_TEST_TIMEOUT 300)  # 5 minutes timeout per test

# Test configuration
option(NLM_ENABLE_UNIT_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
option(NLM_ENABLE_PERFORMANCE_TESTS "Enable performance tests" OFF)
option(NLM_ENABLE_PYTHON_TESTS "Enable Python bindings tests" ON)
option(NLM_ENABLE_VALGRIND_TESTS "Enable Valgrind memory checking" OFF)
option(NLM_ENABLE_ASAN_TESTS "Enable AddressSanitizer tests" OFF)
option(NLM_ENABLE_MSAN_TESTS "Enable MemorySanitizer tests" OFF)

# Test directories
set(NLM_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tests)
set(NLM_TEST_HELPERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test_helpers)

# Test configuration files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../test_config.cmake.in
    ${CMAKE_BINARY_DIR}/test_config.cmake
    @ONLY
)

# Base test configuration
include(${CMAKE_BINARY_DIR}/test_config.cmake)

# Test compiler and standard requirements
function(add_nlm_test test_name source_files)
    add_executable(${test_name} ${source_files})
    target_link_libraries(${test_name} PRIVATE nlm_core)
    target_compile_features(${test_name} PRIVATE cxx_std_20)
    
    # Apply test-specific settings
    if(NLM_ENABLE_VALGRIND_TESTS)
        target_compile_options(${test_name} PRIVATE -fsanitize=address)
        target_link_options(${test_name} PRIVATE -fsanitize=address)
    endif()
    
    if(NLM_ENABLE_ASAN_TESTS)
        target_compile_options(${test_name} PRIVATE -fsanitize=address)
        target_link_options(${test_name} PRIVATE -fsanitize=address)
    endif()
    
    if(NLM_ENABLE_MSAN_TESTS)
        target_compile_options(${test_name} PRIVATE -fsanitize=memory)
        target_link_options(${test_name} PRIVATE -fsanitize=memory)
    endif()
    
    add_test(NAME ${test_name}
        COMMAND ${test_name}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    
    # Add test to ctest with verbose output
    set_tests_properties(${test_name} PROPERTIES
        LABELS "unit;test"
        TIMEOUT ${CTEST_TEST_TIMEOUT}
    )
endfunction()

# Add unit tests for core components
add_nlm_test(nlm_test_core
    test_main.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
)

add_nlm_test(nlm_test_brain
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

add_nlm_test(nlm_test_plasticity
    test_hebbian.cpp
    test_structural_plasticity.cpp
)

add_nlm_test(nlm_test_memory
    test_working_memory.cpp
    test_episodic_memory.cpp
    test_associative_memory.cpp
)

add_nlm_test(nlm_test_cognition
    test_concept_formation.cpp
    test_neural_planner.cpp
)

add_nlm_test(nlm_test_performance
    test_performance.cpp
    test_parallel_processing.cpp
)

# Integration tests
if(NLM_ENABLE_INTEGRATION_TESTS)
    add_nlm_test(nlm_test_integration
        test_integration.cpp
        test_world.cpp
        test_agent.cpp
    )
    
    set_tests_properties(nlm_test_integration PROPERTIES
        LABELS "integration;system"
    )
endif()

# Phase-specific tests
if(NLM_ENABLE_PHASE3_EXPERIMENTS)
    add_nlm_test(nlm_test_phase3
        test_phase3.cpp
    )
    set_tests_properties(nlm_test_phase3 PROPERTIES
        LABELS "phase3;experiment"
    )
endif()

if(NLM_ENABLE_PHASE4_EXPERIMENTS)
    add_nlm_test(nlm_test_phase4
        test_phase4.cpp
    )
    set_tests_properties(nlm_test_phase4 PROPERTIES
        LABELS "phase4;experiment"
    )
endif()

# Performance benchmarks
if(NLM_ENABLE_PERFORMANCE_TESTS)
    add_nlm_test(nlm_benchmark_neuron
        benchmark_neuron.cpp
    )
    set_tests_properties(nlm_benchmark_neuron PROPERTIES
        LABELS "benchmark;performance"
        SKIP_REGULAR_EXPRESSION "SKIP_BENCHMARKS"
    )
    
    add_nlm_test(nlm_benchmark_plasticity
        benchmark_plasticity.cpp
    )
    set_tests_properties(nlm_benchmark_plasticity PROPERTIES
        LABELS "benchmark;plasticity"
        SKIP_REGULAR_EXPRESSION "SKIP_BENCHMARKS"
    )
endif()

# Test coverage configuration
if(NLM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Coverage.cmake)
    coverage_setup("nlm_coverage")
endif()

# Test results analysis and reporting
add_custom_target(test_results
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests with verbose output"
    VERBATIM
)

# Generate test report
add_custom_target(generate_test_report
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/GenerateTestReport.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating test report"
    VERBATIM
)

# Add test dependency validation
set(TEST_DEPENDENCIES
    "nlm_core"
)

foreach(dep ${TEST_DEPENDENCIES})
    add_dependencies(${test_name} ${dep})
endforeach()

# Test data management
file(WRITE ${CMAKE_BINARY_DIR}/test_data/CMakeLists.txt
    "# Test data configuration
add_custom_target(copy_test_data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test_data
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../test_data
        ${CMAKE_BINARY_DIR}/test_data
    COMMENT "Copying test data to build directory"
)
")

# Test environment setup
add_custom_target(test_setup
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test_environment
    COMMAND echo "# Test environment configuration" > ${CMAKE_BINARY_DIR}/test_environment/config.env
    COMMAND echo "NLM_TEST_MODE=unit" >> ${CMAKE_BINARY_DIR}/test_environment/config.env
    COMMAND echo "NLM_RANDOM_SEED=42" >> ${CMAKE_BINARY_DIR}/test_environment/config.env
    COMMAND echo "NLM_SIMULATION_TIMESTEP=0.001" >> ${CMAKE_BINARY_DIR}/test_environment/config.env
    COMMENT "Setting up test environment"
)

# Test result aggregation
add_custom_target(aggregate_test_results
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/AggregateTestResults.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Aggregating test results"
    VERBATIM
)

# Add test timeout handling
add_custom_target(test_timeout_handler
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/TimeoutHandler.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Handling test timeouts"
    VERBATIM
)

message(STATUS "Testing framework configured:")
message(STATUS "  Unit tests: ${NLM_ENABLE_UNIT_TESTS}")
message(STATUS "  Integration tests: ${NLM_ENABLE_INTEGRATION_TESTS}")
message(STATUS "  Performance tests: ${NLM_ENABLE_PERFORMANCE_TESTS}")
message(STATUS "  Python tests: ${NLM_ENABLE_PYTHON_TESTS}")
message(STATUS "  Valgrind tests: ${NLM_ENABLE_VALGRIND_TESTS}")
message(STATUS "  AddressSanitizer: ${NLM_ENABLE_ASAN_TESTS}")
message(STATUS "  MemorySanitizer: ${NLM_ENABLE_MSAN_TESTS}")
message(STATUS "  Test timeout: ${CTEST_TEST_TIMEOUT}s")
