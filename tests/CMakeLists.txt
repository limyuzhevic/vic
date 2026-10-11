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
    test_config_edge.cpp
    test_brain_edge.cpp
    test_neuron_edge.cpp
    test_memory_edge.cpp
    test_error_handling.cpp
    test_timing_edge.cpp
    test_plasticity_edge.cpp
    test_boundary.cpp
    test_memory_safety.cpp
    test_performance.cpp
    test_integration.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
