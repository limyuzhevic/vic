# Comprehensive NLM Test Configuration
# Build system for all enhanced test suites

cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../tests)

# Core test executable (existing tests)
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
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Unit tests executable (enhanced comprehensive tests)
add_executable(nlm_unit_tests
    unit/test_framework.cpp
    unit/test_config.cpp
    unit/test_brain.cpp
    unit/test_agentbrain.cpp
    unit/test_simpleworld.cpp
    unit/test_advanced_actions.cpp
    unit/test_plugin.cpp
    unit/test_python_bindings.cpp
)

target_link_libraries(nlm_unit_tests PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(nlm_unit_tests PRIVATE cxx_std_20)

# Integration tests executable
add_executable(nlm_integration_tests
    integration/test_integration.cpp
)

target_link_libraries(nlm_integration_tests PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(nlm_integration_tests PRIVATE cxx_std_20)

# Performance tests executable (compile only, runtime tests separate)
add_executable(nlm_performance_tests
    performance/benchmark.cpp
    performance/stress_test.cpp
    performance/memory_test.cpp
)

target_link_libraries(nlm_performance_tests PRIVATE nlm_core)
target_compile_features(nlm_performance_tests PRIVATE cxx_std_20)

# Compatibility tests executable
add_executable(nlm_compatibility_tests
    compatibility/api_compatibility_test.cpp
    compatibility/config_compatibility_test.cpp
    compatibility/behavior_compatibility_test.cpp
)

target_link_libraries(nlm_compatibility_tests PRIVATE nlm_core)
target_compile_features(nlm_compatibility_tests PRIVATE cxx_std_20)

# Quality tests executable
add_executable(nlm_quality_tests
    quality/code_quality_test.cpp
    quality/documentation_test.cpp
    quality/error_handling_test.cpp
    quality/edge_case_test.cpp
)

target_link_libraries(nlm_quality_tests PRIVATE nlm_core)
target_compile_features(nlm_quality_tests PRIVATE cxx_std_20)

# Enable testing
enable_testing()

# Add all tests to test suite
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME nlm_unit_tests COMMAND nlm_unit_tests)
add_test(NAME nlm_integration_tests COMMAND nlm_integration_tests)
add_test(NAME nlm_performance_tests COMMAND nlm_performance_tests)
add_test(NAME nlm_compatibility_tests COMMAND nlm_compatibility_tests)
add_test(NAME nlm_quality_tests COMMAND nlm_quality_tests)

# Python tests (would be handled separately)
# Note: Python tests require pytest and the NLM Python package to be installed

EOF
