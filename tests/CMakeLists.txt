cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create comprehensive test executable
add_executable(nlm_test
    test_main.cpp
    test_main_comprehensive.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    
    # Unit tests
    unit/test_config_management.cpp
    unit/test_performance_monitoring.cpp
    unit/test_experiment_management.cpp
    unit/test_neuron_advanced.cpp
    unit/test_stdp_advanced.cpp
    unit/test_agent_brain_enhanced.cpp
    
    # Integration tests
    integration/test_system_integration.cpp
    integration/test_python_bindings.cpp
    integration/test_cli_integration.cpp
    
    # Performance tests
    performance/test_performance_benchmarks.cpp
    performance/test_scalability_tests.cpp
    performance/test_memory_usage.cpp
    
    # Compatibility tests
    compatibility/test_backward_compatibility.cpp
    compatibility/test_data_format.cpp
    compatibility/test_configuration.cpp
    
    # Stress tests
    stress/test_large_scale_simulation.cpp
    stress/test_memory_extremes.cpp
    stress/test_performance_extremes.cpp
    
    # Test utilities
    utils/TestUtils.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

add_test(NAME nlm_test COMMAND nlm_test)
