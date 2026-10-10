cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

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
    test_edge_cases.cpp
    test_memory_management.cpp
    test_plasticity_edge_cases.cpp
    test_neuromodulation_edge_cases.cpp
    test_configuration_edge_cases.cpp
    test_integration_edge_cases.cpp
    test_performance_edge_cases.cpp
    test_development_edge_cases.cpp
    test_utils.cpp
    test_performance_utils.cpp
    test_memory_utils.cpp
)

# Add additional source files needed by test utilities
add_executable(nlm_test_utilities
    tests/test_utils.cpp
    tests/test_performance_utils.cpp
    tests/test_memory_utils.cpp
)

# Link test libraries
target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
