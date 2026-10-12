# CMakeLists.txt for NLM test suite
# Provides build configuration for all test executables and test utilities

cmake_minimum_required(VERSION 3.16)
project(NLMTests VERSION 0.1.0 LANGUAGES CXX)

# Test configuration
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(ENABLE_TEST_COVERAGE "Enable code coverage" OFF)

# Compiler flags for tests
if(MSVC)
    add_compile_options(/W4 /WX /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -std=c++20)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/brain)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/agent)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/world)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/dynamics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/development)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/neumodulation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/memory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/prediction)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/motor)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/environment)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/visualization)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/test)

# Test dependencies
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

# Test source files
set(TEST_SOURCES
    tests/test_brain.cpp
    tests/test_clock.cpp
    tests/test_config.cpp
    tests/test_main.cpp
    tests/test_neuron.cpp
    tests/test_random.cpp
    tests/test_stdp.cpp
    tests/test_synapse.cpp
    tests/test_types.cpp
)

# Test utility sources
set(TEST_UTILITY_SOURCES
    test/TestUtils.hpp
    test/TestUtils.cpp
)

# Example test sources
set(EXAMPLE_TEST_SOURCES
    test/TestExamples.cpp
)

# Test headers
set(TEST_HEADERS
    test/TestUtils.hpp
)

# Test executables
set(TEST_TARGETS
    nlm_test_brain
    nlm_test_config
    nlm_test_neuron
    nlm_test_stdp
    nlm_test_synapse
    nlm_test_integration
    nlm_test_examples
)

# Benchmark targets (optional)
if(BUILD_BENCHMARKS)
    set(BENCHMARK_SOURCES
        test/TestBenchmarks.cpp
    )
    
    set(BENCHMARK_TARGETS
        brain_benchmark
        simulation_benchmark
        plasticity_benchmark
        memory_benchmark
        action_benchmark
    )
endif()

# Function to create test executables
function(add_nlm_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE nlm_core GTest::gtest GTest::gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    if(ENABLE_TEST_COVERAGE)
        target_compile_options(${test_name} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_libraries(${test_name} PRIVATE --coverage)
    endif()
    
    # Add dependencies
    target_link_libraries(${test_name} PRIVATE nlm_core)
    
    # Add test dependencies
    target_link_libraries(${test_name} PRIVATE
        nlm_agent
        nlm_world
        nlm_phase3
        nlm_phase4
    )
endfunction()

# Function to create example test executable
function(add_example_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE nlm_core GTest::gtest GTest::gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    if(ENABLE_TEST_COVERAGE)
        target_compile_options(${test_name} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_libraries(${test_name} PRIVATE --coverage)
    endif()
    
    target_link_libraries(${test_name} PRIVATE nlm_core)
endfunction()

# Create test executables
if(BUILD_TESTS)
    # Basic component tests
    add_nlm_test(nlm_test_brain tests/test_brain.cpp)
    add_nlm_test(nlm_test_config tests/test_config.cpp)
    add_nlm_test(nlm_test_neuron tests/test_neuron.cpp)
    add_nlm_test(nlm_test_stdp tests/test_stdp.cpp)
    add_nlm_test(nlm_test_synapse tests/test_synapse.cpp)
    
    # Utility tests
    add_nlm_test(nlm_test_types tests/test_types.cpp)
    add_nlm_test(nlm_test_clock tests/test_clock.cpp)
    add_nlm_test(nlm_test_random tests/test_random.cpp)
    add_nlm_test(nlm_test_main tests/test_main.cpp)
    
    # Integration test
    add_nlm_test(nlm_test_integration tests/integration_test.cpp)
endif()

# Create example test
if(BUILD_TESTS)
    add_example_test(nlm_test_examples test/TestExamples.cpp)
endif()

# Create benchmark executables
if(BUILD_BENCHMARKS)
    add_executable(brain_benchmark test/TestBenchmarks.cpp)
    target_link_libraries(brain_benchmark PRIVATE nlm_core benchmark::benchmark)
    add_test(NAME brain_benchmark COMMAND brain_benchmark)
    
    add_executable(simulation_benchmark test/TestBenchmarks.cpp)
    target_link_libraries(simulation_benchmark PRIVATE nlm_core benchmark::benchmark)
    add_test(NAME simulation_benchmark COMMAND simulation_benchmark)
    
    add_executable(plasticity_benchmark test/TestBenchmarks.cpp)
    target_link_libraries(plasticity_benchmark PRIVATE nlm_core benchmark::benchmark)
    add_test(NAME plasticity_benchmark COMMAND plasticity_benchmark)
    
    add_executable(memory_benchmark test/TestBenchmarks.cpp)
    target_link_libraries(memory_benchmark PRIVATE nlm_core benchmark::benchmark)
    add_test(NAME memory_benchmark COMMAND memory_benchmark)
    
    add_executable(action_benchmark test/TestBenchmarks.cpp)
    target_link_libraries(action_benchmark PRIVATE nlm_core benchmark::benchmark)
    add_test(NAME action_benchmark COMMAND action_benchmark)
endif()

# Test macros and utilities
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test/TestMacros.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/test/TestMacros.hpp
)

# Installation targets
install(TARGETS ${TEST_TARGETS}
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)

if(BUILD_BENCHMARKS)
    install(TARGETS ${BENCHMARK_TARGETS}
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib)
endif()

# Copy test headers to installation
install(FILES ${TEST_HEADERS}
        DESTINATION include/nlm/test)

# Add custom targets for running tests
add_custom_target(run_all_tests
    COMMAND ctest --output-on-failure
    DEPENDS ${TEST_TARGETS}
    COMMENT "Running all tests..."
)

add_custom_target(run_examples
    COMMAND nlm_test_examples
    DEPENDS nlm_test_examples
    COMMENT "Running example tests..."
)

add_custom_target(run_benchmarks
    COMMAND brain_benchmark
    COMMAND simulation_benchmark
    COMMAND plasticity_benchmark
    COMMAND memory_benchmark
    COMMAND action_benchmark
    DEPENDS ${BENCHMARK_TARGETS}
    COMMENT "Running benchmarks..."
)

# Test coverage reporting
if(ENABLE_TEST_COVERAGE)
    find_program(GCOVR_PATH gcovr PATH)
    if(GCOVR_PATH)
        add_custom_target(coverage
            COMMAND ${GCOVR_PATH} --root . --exclude *.hpp --exclude */build/* -r html
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating test coverage report..."
        )
    endif()
endif()

# Summary
message(STATUS "Test configuration complete:")
message(STATUS "  - Number of test targets: ${TEST_TARGETS}")
message(STATUS "  - Number of benchmark targets: ${BENCHMARK_TARGETS}")
message(STATUS "  - Test coverage enabled: ${ENABLE_TEST_COVERAGE}")
message(STATUS "  - Build benchmarks enabled: ${BUILD_BENCHMARKS}")

# Enable testing
enable_testing()