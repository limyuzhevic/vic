cmake_minimum_required(VERSION 3.16)
project(NLM_TESTS CXX)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Find GoogleTest
find_package(GTest REQUIRED)

# Create comprehensive test executable with GoogleTest
add_executable(nlm_test_comprehensive
    test_brain_gtest.cpp
    test_neuron_gtest.cpp
    test_memory_gtest.cpp
    test_neuromodulation_gtest.cpp
    test_plasticity_gtest.cpp
    test_development_gtest.cpp
    test_prediction_gtest.cpp
    test_cognition_gtest.cpp
    test_integration_gtest.cpp
    test_utils_gtest.cpp
)

target_link_libraries(nlm_test_comprehensive PRIVATE nlm_core GTest::gtest GTest::gtest_main)
target_compile_features(nlm_test_comprehensive PRIVATE cxx_std_20)

# Create legacy test executable (traditional assertion-based tests)
add_executable(nlm_test_legacy
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    NeuromodulatorTest.cpp
    test_json_loading.cpp
)

target_link_libraries(nlm_test_legacy PRIVATE nlm_core)
target_compile_features(nlm_test_legacy PRIVATE cxx_std_20)

# Create unit test executable
add_executable(nlm_test_unit
    test_brain_gtest.cpp
    test_neuron_gtest.cpp
    test_memory_gtest.cpp
    test_neuromodulation_gtest.cpp
    test_plasticity_gtest.cpp
    test_development_gtest.cpp
    test_prediction_gtest.cpp
    test_cognition_gtest.cpp
    test_utils_gtest.cpp
)

target_link_libraries(nlm_test_unit PRIVATE nlm_core GTest::gtest GTest::gtest_main)
target_compile_features(nlm_test_unit PRIVATE cxx_std_20)

# Create integration test executable
add_executable(nlm_test_integration
    test_integration_gtest.cpp
)

target_link_libraries(nlm_test_integration PRIVATE nlm_core GTest::gtest GTest::gtest_main)
target_compile_features(nlm_test_integration PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test_comprehensive COMMAND nlm_test_comprehensive)
add_test(NAME nlm_test_legacy COMMAND nlm_test_legacy)
add_test(NAME nlm_test_unit COMMAND nlm_test_unit)
add_test(NAME nlm_test_integration COMMAND nlm_test_integration)
