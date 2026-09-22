cmake_minimum_required(VERSION 3.16)

# Test framework setup with modern CMake practices and Google Test
project(nlm_tests LANGUAGES CXX)

# Configure C++ standard and warnings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type for tests" FORCE)
endif()

# Enable testing framework
enable_testing()

# Modern CMake: Create test target with target-based dependencies
# This approach provides better dependency management and cleaner build

# Add Google Test if not already available
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# For Windows: Prevent overriding of parent project's compiler/linker settings
if(MSVC)
    set(googletest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

# Download and make available
FetchContent_MakeAvailable(googletest)

# Include test framework headers
target_include_directories(nlm_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Function to create a test using Google Test patterns
function(nlm_add_test_gtest test_name test_source)
    # Create test executable using Google Test
    add_executable(${test_name} ${test_source})
    
    # Set up target properties consistently
    target_compile_features(${test_name} PRIVATE cxx_std_20)
    
    # Add test-specific compile options
    if(MSVC)
        target_compile_options(${test_name} PRIVATE /permissive- /W4)
    else()
        target_compile_options(${test_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    
    # Link against main NLM library
    target_link_libraries(${test_name} PRIVATE nlm_core gtest_main)
    
    # Add test-specific include directories
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Enable test coverage if requested
    if(ENABLE_COVERAGE)
        if(NOT MSVC)
            target_compile_options(${test_name} PRIVATE --coverage -O0 -g)
            target_link_options(${test_name} PRIVATE --coverage)
        endif()
    endif()
    
    # Add to CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # Set up test properties
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
        EXCLUDE_FROM_ALL FALSE
    )
    
    message(STATUS "Added Google Test: ${test_name}")
endfunction()

# Create test suite with modular organization
# Group tests by functionality for better maintainability

# Core system tests
nlm_add_test_gtest(nlm_test_core test_core.cpp)

# Brain component tests
nlm_add_test_gtest(nlm_test_brain test_brain.cpp)

# Plasticity tests
nlm_add_test_gtest(nlm_test_plasticity test_plasticity.cpp)

# Development tests
nlm_add_test_gtest(nlm_test_development test_development.cpp)

# Neuromodulation tests
nlm_add_test_gtest(nlm_test_neuromodulation test_neuromodulation.cpp)

# Memory tests
nlm_add_test_gtest(nlm_test_memory test_memory.cpp)

# Cognition tests
nlm_add_test_gtest(nlm_test_cognition test_cognition.cpp)

# Sensory tests
nlm_add_test_gtest(nlm_test_sensory test_sensory.cpp)

# Prediction tests
nlm_add_test_gtest(nlm_test_prediction test_prediction.cpp)

# Motor tests
nlm_add_test_gtest(nlm_test_motor test_motor.cpp)

# Environment tests
nlm_add_test_gtest(nlm_test_environment test_environment.cpp)

# Experimental tests
nlm_add_test_gtest(nlm_test_experiments test_experiments.cpp)

# Integration tests
nlm_add_test_gtest(nlm_test_integration test_integration.cpp)

# Performance tests (separate target for benchmarking)
if(BUILD_PERFORMANCE_TESTS)
    add_executable(nlm_benchmark_benchmark benchmark.cpp)
    target_compile_features(nlm_benchmark_benchmark PRIVATE cxx_std_20)
    target_link_libraries(nlm_benchmark_benchmark PRIVATE nlm_core)
    target_include_directories(nlm_benchmark_benchmark PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    add_test(NAME nlm_benchmark BENCHMARK nlm_benchmark_benchmark)
endif()

# Mock and stub setup for testing (basic implementation)
function(add_mock_library mock_name)
    add_library(${mock_name} STATIC ${ARGN})
    target_include_directories(${mock_name} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    )
    set_target_properties(${mock_name} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        EXCLUDE_FROM_ALL TRUE
    )
endfunction()

# Test configuration summary
if(TEST_COVERAGE_ENABLED)
    message(STATUS "Test coverage enabled")
    target_compile_options(gtest_coverage INTERFACE --coverage -O0 -g)
endif()

if(BUILD_PERFORMANCE_TESTS)
    message(STATUS "Performance tests enabled")
endif()

# Install test binaries for external testing (optional)
install(TARGETS nlm_test_core nlm_test_brain nlm_test_plasticity nlm_test_development
    nlm_test_neuromodulation nlm_test_memory nlm_test_cognition nlm_test_sensory
    nlm_test_prediction nlm_test_motor nlm_test_environment nlm_test_experiments
    nlm_test_integration
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/tests
)

# Summary output
message(STATUS "NLM test suite configured with Google Test")
message(STATUS "- Core library target: nlm_core")
message(STATUS "- Tests defined: ${CMAKE_CURRENT_BINARY_DIR}/bin")
message(STATUS "- Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "- Google Test included: ${GTEST_AVAILABLE}")
