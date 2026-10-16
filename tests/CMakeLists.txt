cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Test sources

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
    test_brain_neural_core.cpp
    test_brain_plasticity_manager.cpp
    test_brain_memory_manager.cpp
    test_brain_cognitive_manager.cpp
    test_brain_neuromodulation_manager.cpp
    test_brain_development_manager.cpp
    test_brain_simulation_manager.cpp
)

# Create test executable
add_executable(nlm_test ${TEST_SOURCES})

target_link_libraries(nlm_test PRIVATE nlm_core nlm_brain nlm_memory nlm_prediction nlm_cognition nlm_neuromodulation nlm_development)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
