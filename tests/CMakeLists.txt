cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create test executable
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
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Create integration test executable
add_executable(nlm_integration_test
    test_integration_framework.cpp
    test_integration_framework.hpp
    test_suite_runner.hpp
    test_suite_runner.cpp
    test_integration_suite.cpp
    test/test_phase2_integration.cpp
    test/test_phase3_integration.cpp
    test/test_phase4_integration.cpp
    test/test_phase5_integration.cpp
    test/test_phase6_integration.cpp
)

target_link_libraries(nlm_integration_test PRIVATE nlm_core)
target_compile_features(nlm_integration_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME nlm_integration_test COMMAND nlm_integration_test)
