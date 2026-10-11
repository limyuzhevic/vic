cmake_minimum_required(VERSION 3.16)

# NLM Comprehensive Testing Framework CMake Configuration

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directories (relative path to test framework files)
set(NLM_TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cf14c76c-4b1f-4d38-9a96-64cd8a400481/src")

# Comprehensive test executable
add_executable(nlm_comprehensive_test
    # Test source files (legacy)
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    
    # Comprehensive test framework (NLM test framework)
    ${NLM_TEST_SOURCE_DIR}/nlm_test_framework.cpp
    ${NLM_TEST_SOURCE_DIR}/nlm_test_framework.hpp
    ${NLM_TEST_SOURCE_DIR}/nlm_unit_tests.cpp
    ${NLM_TEST_SOURCE_DIR}/nlm_performance_tests.cpp
    ${NLM_TEST_SOURCE_DIR}/nlm_integration_tests.cpp
    ${NLM_TEST_SOURCE_DIR}/nlm_regression_tests.cpp
    ${NLM_TEST_SOURCE_DIR}/nlm_test_integration.hpp
    ${NLM_TEST_SOURCE_DIR}/nlm_test_runner.hpp
)

target_link_libraries(nlm_comprehensive_test PRIVATE nlm_core)
target_include_directories(nlm_comprehensive_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${NLM_TEST_SOURCE_DIR}
)

# Legacy test executable (for backward compatibility)
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

target_link_libraries(nlm_test PRIVATE nlm_core)

# Enable testing
enable_testing()

# Add comprehensive test to ctest
add_test(NAME nlm_comprehensive_test COMMAND nlm_comprehensive_test)

# Add legacy test to ctest
add_test(NAME nlm_test COMMAND nlm_test)

# Installation targets (optional)
install(TARGETS nlm_comprehensive_test nlm_test
    RUNTIME DESTINATION bin
)
