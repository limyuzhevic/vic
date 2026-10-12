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
    test_refactored_agentbrain.cpp
    test_performance.cpp
    test_agentbrain.cpp
    test_config_enhanced.cpp
    test_visualization.cpp
    test_metrics.cpp
    test_research_tools.cpp
    test_integration.cpp
    test_e2e.cpp
    test_benchmarks.cpp
    test_framework.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
