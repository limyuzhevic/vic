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
    test_working_memory.cpp
    test_episodic_memory.cpp
    test_concept_formation.cpp
    test_attention.cpp
    test_planning.cpp
    test_self_model.cpp
    test_social_learning.cpp
    test_phase6_integration.cpp
    test_system_integration.cpp
    test_performance_benchmarks.cpp
    test_edge_cases.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
