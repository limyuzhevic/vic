cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create enhanced test executable
add_executable(nlm_test_enhanced
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_development_integration.cpp
    test_error_handling.cpp
    test_phase6_integration.cpp
    test_python_bindings.cpp
    test_python_integration.cpp
    test_development.cpp
    test_neuromodulation.cpp
    test_memory.cpp
    test_prediction.cpp
    test_performance.cpp
    test_export_import.cpp
    test_tools.cpp
)

target_link_libraries(nlm_test_enhanced PRIVATE nlm_core)
target_compile_features(nlm_test_enhanced PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test_enhanced COMMAND nlm_test_enhanced)

# Optional: Create a basic test executable for backward compatibility
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

# Enable testing for basic tests
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)