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
    
    # Phase 1: Plasticity System Integration Tests
    test_stdp_integration.cpp
    test_hebbian_integration.cpp
    test_structural_plasticity_integration.cpp
    
    # Phase 2: Memory System Integration Tests
    test_working_memory_integration.cpp
    test_episodic_memory_integration.cpp
    test_associative_memory_integration.cpp
    
    # Phase 3: Neuromodulation Integration Tests
    test_prediction_error_integration.cpp
    test_novelty_integration.cpp
    test_curiosity_integration.cpp
    
    # Phase 4: Cross-System Integration Tests
    test_plasticity_memory_integration.cpp
    test_neuromodulation_plasticity_integration.cpp
    test_memory_neuromodulation_integration.cpp
    
    # Phase 5: Full Brain Integration Tests
    test_full_brain_integration.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
