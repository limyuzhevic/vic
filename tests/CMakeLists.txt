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
    
    # Phase 6 integration tests
    test_phase6_integration.cpp
    test_phase6_memory.cpp
    test_phase6_neuromodulation.cpp
    test_phase6_prediction.cpp
    test_phase6_development.cpp
    test_phase6_checkpoint.cpp
    test_phase6_replay.cpp
    
    # Comprehensive test suite
    test_full_integration.cpp
    test_performance_benchmark.cpp
    test_configuration_validation.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
