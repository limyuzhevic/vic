cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executables for different test categories

# Unit tests
add_executable(nlm_unit_tests
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_unit_tests PRIVATE nlm_core)
target_compile_features(nlm_unit_tests PRIVATE cxx_std_20)

# Integration tests
add_executable(nlm_integration_tests
    integration/test_brain_integration.cpp
    integration/test_memory_integration.cpp
    integration/test_neuromodulation_integration.cpp
    integration/test_agent_integration.cpp
)

target_link_libraries(nlm_integration_tests PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(nlm_integration_tests PRIVATE cxx_std_20)

# Performance benchmarks
add_executable(nlm_performance_benchmarks
    performance/benchmark_network_sizes.cpp
    performance/benchmark_memory_usage.cpp
)

target_link_libraries(nlm_performance_benchmarks PRIVATE nlm_core)
target_compile_features(nlm_performance_benchmarks PRIVATE cxx_std_20)

# Test framework utilities
add_executable(nlm_test_framework
    performance/utilities/performance_metrics.cpp
    performance/utilities/test_framework.cpp
)

target_link_libraries(nlm_test_framework PRIVATE nlm_core)
target_compile_features(nlm_test_framework PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME unit_tests COMMAND nlm_unit_tests)
add_test(NAME integration_tests COMMAND nlm_integration_tests)
add_test(NAME performance_benchmarks COMMAND nlm_performance_benchmarks)

# Add test targets for IDE support
add_custom_target(run_unit_tests COMMAND nlm_unit_tests)
add_custom_target(run_integration_tests COMMAND nlm_integration_tests)
add_custom_target(run_performance_benchmarks COMMAND nlm_performance_benchmarks)
add_custom_target(run_all_tests COMMAND nlm_unit_tests nlm_integration_tests nlm_performance_benchmarks)
