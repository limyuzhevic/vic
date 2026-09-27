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
    test_hebbian.cpp
    test_structural_plasticity.cpp
    test_synaptogenesis.cpp
    test_pruning.cpp
    test_maturation.cpp
    test_curiosity.cpp
    test_novelty.cpp
    test_prediction_error.cpp
    test_working_memory.cpp
    test_episodic_memory.cpp
    test_associative_memory.cpp
    test_integration.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
