cmake_minimum_required(VERSION 3.16)

# Comprehensive NLM Testing Framework Configuration
# Phase 3: Comprehensive Test Infrastructure

project(nlm_comprehensive_tests LANGUAGES CXX)

# Test framework configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing and coverage
enable_testing()

# Include testing framework
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Test compiler flags for comprehensive testing
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Create test executables for different categories
add_executable(nlm_unit_tests
    unit/TestBrainCore.cpp
    unit/TestMemorySystem.cpp
    unit/TestNeuromodulation.cpp
    unit/TestPlasticity.cpp
    unit/TestCognition.cpp
    unit/TestSensory.cpp
    unit/TestMotor.cpp
    unit/TestPerformance.cpp
    unit/TestConfig.cpp
    unit/TestDevelopment.cpp
)

add_executable(nlm_integration_tests
    integration/TestSystemIntegration.cpp
    integration/TestPluginIntegration.cpp
    integration/TestPerformanceIntegration.cpp
    integration/TestScalability.cpp
)

add_executable(nlm_performance_tests
    performance/TestBenchmarks.cpp
    performance/TestMemoryUsage.cpp
    performance/TestCPUPerformance.cpp
)

add_executable(nlm_stress_tests
    stress/TestLargeScaleSimulation.cpp
    stress/TestMemoryPressure.cpp
    stress/TestConcurrency.cpp
    stress/TestResourceExhaustion.cpp
)

add_executable(nlm_compatibility_tests
    compatibility/TestPlatformCompatibility.cpp
    compatibility/TestPythonBindings.cpp
    compatibility/TestPluginCompatibility.cpp
    compatibility/TestConfigurationCompatibility.cpp
)

add_executable(nlm_regression_tests
    regression/TestBaselineResults.cpp
    regression/TestVersionCompatibility.cpp
    regression/TestDataFormatCompatibility.cpp
)

add_executable(nlm_framework_tests
    framework/TestFramework.hpp
    framework/TestFixtures.hpp
    framework/TestDataGenerator.hpp
    framework/TestReportGenerator.hpp
)

add_executable(nlm_benchmarks
    benchmarks/BenchmarkMemory.cpp
    benchmarks/BenchmarkCPU.cpp
    benchmarks/BenchmarkNetwork.cpp
    benchmarks/BenchmarkIntegration.cpp
)

add_executable(nlm_profiling_tests
    profiling/TestProfilingAccuracy.cpp
    profiling/TestPerformanceProfiler.cpp
)

# Link test executables with required libraries
foreach(target nlm_unit_tests nlm_integration_tests nlm_performance_tests nlm_stress_tests
        nlm_compatibility_tests nlm_regression_tests nlm_framework_tests nlm_benchmarks
        nlm_profiling_tests)
    target_link_libraries(${target} PRIVATE nlm_core)
    target_compile_features(${target} PRIVATE cxx_std_20)
endforeach()

# Test configuration files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.cmake.in
    ${CMAKE_CURRENT_SOURCE_DIR}/test_config.cmake
    @ONLY
)

# Add tests to CTest
add_test(NAME unit_tests COMMAND nlm_unit_tests)
add_test(NAME integration_tests COMMAND nlm_integration_tests)
add_test(NAME performance_tests COMMAND nlm_performance_tests)
add_test(NAME stress_tests COMMAND nlm_stress_tests)
add_test(NAME compatibility_tests COMMAND nlm_compatibility_tests)
add_test(NAME regression_tests COMMAND nlm_regression_tests)

# Test categories for better reporting
add_custom_target(automated_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS nlm_unit_tests nlm_integration_tests nlm_performance_tests
            nlm_stress_tests nlm_compatibility_tests nlm_regression_tests
)

# Installation of test binaries (for CI/CD)
install(TARGETS nlm_unit_tests nlm_integration_tests nlm_performance_tests
        nlm_stress_tests nlm_compatibility_tests nlm_regression_tests
        nlm_framework_tests nlm_benchmarks nlm_profiling_tests
        RUNTIME DESTINATION bin)

message(STATUS "NLM Comprehensive Testing Framework configured")
message(STATUS "Unit Tests: nlm_unit_tests")
message(STATUS "Integration Tests: nlm_integration_tests")
message(STATUS "Performance Tests: nlm_performance_tests")
message(STATUS "Stress Tests: nlm_stress_tests")
message(STATUS "Compatibility Tests: nlm_compatibility_tests")
message(STATUS "Regression Tests: nlm_regression_tests")
message(STATUS "Framework Tests: nlm_framework_tests")
message(STATUS "Benchmarks: nlm_benchmarks")
message(STATUS "Profiling Tests: nlm_profiling_tests")
