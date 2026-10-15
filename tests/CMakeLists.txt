cmake_minimum_required(VERSION 3.24)

# Test configuration using target-based approach
project(nlm_tests LANGUAGES CXX)

# Set C++ standard
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Create test executable using interface library approach
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
    test_brain_integration.cpp
    test_memory_neuromodulation_integration.cpp
    test_prediction_cognition_integration.cpp
    test_development_learning_integration.cpp
    test_cross_subsystem_coordination.cpp
    test_performance_stress.cpp
    test_performance_monitoring.cpp
    test_persistence.cpp
)

# Link with test framework and project targets
find_package(GTest REQUIRED)

# Use modern GoogleTest approach
include(GoogleTest)

# Enable testing
enable_testing()

# Link with nlm_core and test framework
target_link_libraries(nlm_test PRIVATE nlm_core GTest::gtest GTest::gtest_main)

# Add test to CTest
add_test(NAME nlm_test COMMAND nlm_test)

# Set test properties
add_test(NAME nlm_test COMMAND nlm_test)
set_tests_properties(nlm_test PROPERTIES TIMEOUT 300)

# Better test organization with test suites
# Create a custom test target for better organization
set(TEST_SOURCES
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_brain_integration.cpp
    test_memory_neuromodulation_integration.cpp
    test_prediction_cognition_integration.cpp
    test_development_learning_integration.cpp
    test_cross_subsystem_coordination.cpp
    test_performance_stress.cpp
    test_performance_monitoring.cpp
    test_persistence.cpp
)

add_executable(nlm_test_suite ${TEST_SOURCES})
target_link_libraries(nlm_test_suite PRIVATE nlm_core GTest::gtest GTest::gtest_main)

add_test(NAME nlm_test_suite COMMAND nlm_test_suite)
set_tests_properties(nlm_test_suite PROPERTIES TIMEOUT 300)

# Generate test targets with better organization
# Add test categories as separate targets
function(add_test_category category_name source_files)
    add_executable(${category_name}_test ${source_files})
    target_link_libraries(${category_name}_test PRIVATE nlm_core GTest::gtest GTest::gtest_main)
    add_test(NAME ${category_name}_test COMMAND ${category_name}_test)
    set_tests_properties(${category_name}_test PROPERTIES TIMEOUT 300)
endfunction()

# Example of how to organize tests by category (can be expanded)
add_test_category(cognition test_prediction_cognition_integration.cpp test_development_learning_integration.cpp test_cross_subsystem_coordination.cpp)
add_test_category(memory_neuromodulation test_memory_neuromodulation_integration.cpp test_performance_stress.cpp test_persistence.cpp)
add_test_category(brain_integrated test_brain.cpp test_brain_integration.cpp test_performance_monitoring.cpp)
