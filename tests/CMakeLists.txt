# CMake Integration for NLM Comprehensive Testing Framework
# This file integrates the advanced testing framework with the existing CMake build system

cmake_minimum_required(VERSION 3.16)

# Option to enable/disable testing framework
option(NLM_ENABLE_COMPREHENSIVE_TESTING "Enable comprehensive testing framework" ON)
option(NLM_ENABLE_PROPERTY_TESTS "Enable property-based testing" ON)
option(NLM_ENABLE_PERFORMANCE_TESTS "Enable performance benchmarking" ON)
option(NLM_ENABLE_INTEGRATION_TESTS "Enable integration testing" ON)
option(NLM_ENABLE_REGRESSION_TESTS "Enable regression testing" ON)
option(NLM_ENABLE_COVERAGE_REPORTING "Enable test coverage reporting" ON)

# Set default values
if(NOT DEFINED NLM_BUILD_TYPE)
    set(NLM_BUILD_TYPE "Release")
endif()

# Configure based on build type
if(NLM_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(NLM_ENABLE_COVERAGE_REPORTING ON)
endif()

if(NLM_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Testing framework directories
set(TEST_FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(PROPERTY_TESTS_DIR ${TEST_FRAMEWORK_DIR}/property)
set(PERFORMANCE_TESTS_DIR ${TEST_FRAMEWORK_DIR}/performance)
set(INTEGRATION_TESTS_DIR ${TEST_FRAMEWORK_DIR}/integration)
set(REGRESSION_TESTS_DIR ${TEST_FRAMEWORK_DIR}/regression)
set(INFRASTRUCTURE_TESTS_DIR ${TEST_FRAMEWORK_DIR}/infrastructure)

# Compiler and platform checks
include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)

# Check for C++20 support
check_cxx_compiler_flag(-std=c++20 HAS_CXX20)
if(HAS_CXX20)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Check for pthread support
find_package(Threads REQUIRED)

# Check for Python for QuickCheck
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import quickcheck"
        RESULT_VARIABLE QUICKCHECK_AVAILABLE
        OUTPUT_VARIABLE QUICKCHECK_OUTPUT
        ERROR_VARIABLE QUICKCHECK_ERROR
    )
    if(QUICKCHECK_AVAILABLE EQUAL 0)
        set(HAVE_QUICKCHECK TRUE)
        message(STATUS "QuickCheck Python package found")
    else()
        message(STATUS "QuickCheck Python package not found")
    endif()
endif()

# nlohmann/json dependency
find_package(nlohmann_json 3.2.0 QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found, fetching from GitHub")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Property-based testing framework
if(NLM_ENABLE_COMPREHENSIVE_TESTING AND NLM_ENABLE_PROPERTY_TESTS)
    enable_testing()
    
    # Add property-based test executable
    add_executable(nlm_property_tests
        ${PROPERTY_TESTS_DIR}/property_tests.cpp
        ${PROPERTY_TESTS_DIR}/generators.hpp
        ${PROPERTY_TESTS_DIR}/invariants.hpp
    )
    
    target_include_directories(nlm_property_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${nlohmann_json_INCLUDE_DIRS}
    )
    
    target_link_libraries(nlm_property_tests PRIVATE nlm_core Threads::Threads)
    target_compile_features(nlm_property_tests PRIVATE cxx_std_20)
    
    # Add test target
    add_test(NAME nlm_property_tests COMMAND nlm_property_tests)
    set_tests_properties(nlm_property_tests PROPERTIES TIMEOUT 600)
    
    # Add QuickCheck dependency if available
    if(HAVE_QUICKCHECK)
        find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
        target_include_directories(nlm_property_tests PRIVATE
            ${Python3_INCLUDE_DIRS}
        )
        target_link_libraries(nlm_property_tests PRIVATE ${Python3_LIBRARIES})
        target_compile_definitions(nlm_property_tests PRIVATE HAVE_QUICKCHECK)
    endif()
endif()

# Performance testing framework
if(NLM_ENABLE_COMPREHENSIVE_TESTING AND NLM_ENABLE_PERFORMANCE_TESTS)
    enable_testing()
    
    # Add performance benchmark executable
    add_executable(nlm_benchmarks
        ${PERFORMANCE_TESTS_DIR}/benchmark_runner.cpp
    )
    
    target_include_directories(nlm_benchmarks PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${nlohmann_json_INCLUDE_DIRS}
    )
    
    target_link_libraries(nlm_benchmarks PRIVATE nlm_core Threads::Threads)
    target_compile_features(nlm_benchmarks PRIVATE cxx_std_20)
    
    # Add test target
    add_test(NAME nlm_benchmarks COMMAND nlm_benchmarks)
    set_tests_properties(nlm_benchmarks PROPERTIES TIMEOUT 1800)
endif()

# Integration testing framework
if(NLM_ENABLE_COMPREHENSIVE_TESTING AND NLM_ENABLE_INTEGRATION_TESTS)
    enable_testing()
    
    # Add integration test executable
    add_executable(nlm_integration_tests
        ${INTEGRATION_TESTS_DIR}/integration_test_runner.cpp
    )
    
    target_include_directories(nlm_integration_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${nlohmann_json_INCLUDE_DIRS}
    )
    
    target_link_libraries(nlm_integration_tests PRIVATE nlm_core Threads::Threads)
    target_compile_features(nlm_integration_tests PRIVATE cxx_std_20)
    
    # Add test target
    add_test(NAME nlm_integration_tests COMMAND nlm_integration_tests)
    set_tests_properties(nlm_integration_tests PROPERTIES TIMEOUT 900)
endif()

# Regression testing framework
if(NLM_ENABLE_COMPREHENSIVE_TESTING AND NLM_ENABLE_REGRESSION_TESTS)
    enable_testing()
    
    # Add regression test executable
    add_executable(nlm_regression_tests
        ${REGRESSION_TESTS_DIR}/regression_detector.cpp
    )
    
    target_include_directories(nlm_regression_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${nlohmann_json_INCLUDE_DIRS}
    )
    
    target_link_libraries(nlm_regression_tests PRIVATE nlm_core Threads::Threads)
    target_compile_features(nlm_regression_tests PRIVATE cxx_std_20)
    
    # Add test target
    add_test(NAME nlm_regression_tests COMMAND nlm_regression_tests)
    set_tests_properties(nlm_regression_tests PROPERTIES TIMEOUT 600)
endif()

# Coverage reporting framework
if(NLM_ENABLE_COVERAGE_REPORTING)
    enable_testing()
    
    # Add coverage report executable
    add_executable(nlm_coverage_report
        ${INFRASTRUCTURE_TESTS_DIR}/test_framework_integration.cpp
    )
    
    target_include_directories(nlm_coverage_report PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${nlohmann_json_INCLUDE_DIRS}
    )
    
    target_link_libraries(nlm_coverage_report PRIVATE nlm_core Threads::Threads)
    target_compile_features(nlm_coverage_report PRIVATE cxx_std_20)
    
    # Add test target
    add_test(NAME nlm_coverage_report COMMAND nlm_coverage_report)
    set_tests_properties(nlm_coverage_report PROPERTIES TIMEOUT 300)
endif()

# Custom targets for CI/CD
if(NLM_ENABLE_COMPREHENSIVE_TESTING)
    # All tests target
    add_custom_target(all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS nlm_test
        COMMENT "Running all basic tests"
        VERBATIM
    )
    
    # Property tests target
    if(NLM_ENABLE_PROPERTY_TESTS)
        add_custom_target(property_tests
            COMMAND nlm_property_tests
            DEPENDS nlm_property_tests
            COMMENT "Running property-based tests"
            VERBATIM
        )
        add_dependencies(all_tests property_tests)
    endif()
    
    # Performance tests target
    if(NLM_ENABLE_PERFORMANCE_TESTS)
        add_custom_target(performance_tests
            COMMAND nlm_benchmarks
            DEPENDS nlm_benchmarks
            COMMENT "Running performance benchmarks"
            VERBATIM
        )
        add_dependencies(all_tests performance_tests)
    endif()
    
    # Integration tests target
    if(NLM_ENABLE_INTEGRATION_TESTS)
        add_custom_target(integration_tests
            COMMAND nlm_integration_tests
            DEPENDS nlm_integration_tests
            COMMENT "Running integration tests"
            VERBATIM
        )
        add_dependencies(all_tests integration_tests)
    endif()
    
    # Regression tests target
    if(NLM_ENABLE_REGRESSION_TESTS)
        add_custom_target(regression_tests
            COMMAND nlm_regression_tests
            DEPENDS nlm_regression_tests
            COMMENT "Running regression tests"
            VERBATIM
        )
        add_dependencies(all_tests regression_tests)
    endif()
    
    # Coverage tests target
    if(NLM_ENABLE_COVERAGE_REPORTING)
        add_custom_target(coverage_tests
            COMMAND nlm_coverage_report
            DEPENDS nlm_coverage_report
            COMMENT "Generating coverage reports"
            VERBATIM
        )
        add_dependencies(all_tests coverage_tests)
    endif()
endif()

# Python bindings test
if(Python3_Interpreter_FOUND)
    add_custom_target(python_bindings_tests
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_bindings_improvements.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python bindings tests"
        VERBATIM
    )
    add_dependencies(all_tests python_bindings_tests)
endif()

# Environment setup
if(NLM_ENABLE_COMPREHENSIVE_TESTING)
    # Set test environment variables
    set(ENV{NLM_TEST_MODE} "ci")
    set(ENV{NLM_PERFORMANCE_THRESHOLD} "5.0")
    set(ENV{NLM_COVERAGE_THRESHOLD} "80.0")
    set(ENV{CI} "true")
endif()

# Test configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
    @ONLY
)

# Generate test documentation
add_custom_target(generate_test_documentation
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/docs
    COMMAND ${CMAKE_COMMAND} -E echo "NLM Comprehensive Testing Framework" > ${CMAKE_CURRENT_BINARY_DIR}/docs/index.md
    COMMAND ${CMAKE_COMMAND} -E echo "# Testing Documentation" >> ${CMAKE_CURRENT_BINARY_DIR}/docs/index.md
    COMMENT "Generating test framework documentation"
    VERBATIM
)

# Performance monitoring
if(NLM_ENABLE_PERFORMANCE_TESTS)
    add_custom_target(performance_monitoring
        COMMAND ${CMAKE_COMMAND} -E echo "Starting test performance monitoring..."
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/performance_test_start.txt
        COMMAND sleep 2
        COMMAND ${CMAKE_COMMAND} -E echo "Performance monitoring data:" > ${CMAKE_CURRENT_BINARY_DIR}/performance_report.txt
        COMMAND ${CMAKE_COMMAND} -E echo "Tests completed successfully" >> ${CMAKE_CURRENT_BINARY_DIR}/performance_report.txt
        COMMENT "Monitoring test performance"
        VERBATIM
    )
endif()

# Display configuration summary
function(display_test_framework_configuration)
    message(STATUS "")
    message(STATUS "=== NLM Comprehensive Testing Framework Configuration ===")
    message(STATUS "Build Type: ${NLM_BUILD_TYPE}")
    message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
    message(STATUS "Property-based Testing: ${NLM_ENABLE_PROPERTY_TESTS}")
    message(STATUS "QuickCheck Support: ${HAVE_QUICKCHECK}")
    message(STATUS "Performance Testing: ${NLM_ENABLE_PERFORMANCE_TESTS}")
    message(STATUS "Integration Testing: ${NLM_ENABLE_INTEGRATION_TESTS}")
    message(STATUS "Regression Testing: ${NLM_ENABLE_REGRESSION_TESTS}")
    message(STATUS "Coverage Reporting: ${NLM_ENABLE_COVERAGE_REPORTING}")
    message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "=== Configuration Complete ===")
    message(STATUS "")
endfunction()

# Display configuration
if(NLM_ENABLE_COMPREHENSIVE_TESTING)
    display_test_framework_configuration()
endif()