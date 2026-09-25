cmake_minimum_required(VERSION 3.10)

project(NLMTestingFramework VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Testing framework options
option(NLM_ENABLE_TESTS "Enable NLM tests" ON)
option(NLM_ENABLE_BENCHMARKS "Enable benchmarks" ON)
option(NLM_ENABLE_PROFILING "Enable profiling" ON)
option(NLM_ENABLE_COVERAGE "Enable coverage analysis" OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Testing framework source files
set(TESTING_FRAMEWORK_SOURCES
    TestFramework.cpp
    TestSuite.cpp
    Benchmark.cpp
    Profiler.cpp
    CoverageAnalyzer.cpp
    Assert.cpp
    TestFrameworkDemo.cpp
)

# Create testing framework library
add_library(nlm_testing ${TESTING_FRAMEWORK_SOURCES})

# Set library properties
target_include_directories(nlm_testing PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_testing PUBLIC cxx_std_17)

# Tests
if(NLM_ENABLE_TESTS)
    enable_testing()
    
    # Add tests
    add_executable(test_neuron_demo src/testing/unit/brain/BrainUnitTests.cpp)
    target_link_libraries(test_neuron_demo nlm_testing)
    add_test(NAME test_neuron_demo COMMAND test_neuron_demo)
    
    # Add more test executables...
    # add_executable(test_memory_demo src/testing/unit/memory/MemoryUnitTests.cpp)
    # target_link_libraries(test_memory_demo nlm_testing)
    # add_test(NAME test_memory_demo COMMAND test_memory_demo)
endif()

# Benchmarks
if(NLM_ENABLE_BENCHMARKS)
    add_executable(benchmark_demo src/testing/TestFrameworkDemo.cpp)
    target_link_libraries(benchmark_demo nlm_testing)
    add_custom_target(run_benchmarks COMMAND benchmark_demo)
    
    # Add more benchmark executables...
    # add_executable(neural_benchmarks src/testing/performance/neural/NeuralPerformanceTests.cpp)
    # target_link_libraries(neural_benchmarks nlm_testing)
    # add_custom_target(run_neural_benchmarks COMMAND neural_benchmarks)
endif()

# Installation
install(TARGETS nlm_testing
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testing/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_testingConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_testingConfigVersion.cmake
    DESTINATION lib/cmake/nlm_testing
)

# Summary
message(STATUS "NLM Testing Framework Configuration")
message(STATUS "  Tests enabled: ${NLM_ENABLE_TESTS}")
message(STATUS "  Benchmarks enabled: ${NLM_ENABLE_BENCHMARKS}")
message(STATUS "  Profiling enabled: ${NLM_ENABLE_PROFILING}")
message(STATUS "  Coverage enabled: ${NLM_ENABLE_COVERAGE}")
message(STATUS "  Library: nlm_testing")
message(STATUS "  Include directory: ${CMAKE_CURRENT_SOURCE_DIR}")