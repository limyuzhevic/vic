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
    test_serotonin.cpp
    test_norepinephrine.cpp
    test_memory_retention.cpp
    test_learning_continual.cpp
    test_prediction_integration.cpp
    test_concept_formation.cpp
    test_planner_integration.cpp
    test_attention_integration.cpp
    test_neuromodulation_learning.cpp
    test_sleep_rest_cycle.cpp
    test_system_integration.cpp
    test_phase6_integration.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
