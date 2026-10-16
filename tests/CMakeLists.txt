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
    test_neuron_synapse_config.cpp
    test_suite_runner.cpp
    test_config_integration.cpp
    test_config_validation.cpp
    test_config_presets.cpp
    test_config_serialization.cpp
    test_neuron_integration.cpp
    test_synapse_integration.cpp
    test_brain_config_integration.cpp
    test_validation_framework.cpp
    test_refactored_methods.cpp
    test_backward_compatibility.cpp
    test_performance_regression.cpp
    test_cli_basic.cpp
    test_cli_config.cpp
    test_cli_run.cpp
    test_cli_validation.cpp
    test_examples_compilation.cpp
    test_example_outputs.cpp
    test_example_documentation.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
