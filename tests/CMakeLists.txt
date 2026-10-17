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
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Create comprehensive test executable
add_executable(nlm_comprehensive_test
    comprehensive/test_comprehensive.cpp
    comprehensive/test_memory_safety.cpp
    comprehensive/test_error_handling.cpp
    comprehensive/test_api_correctness.cpp
    comprehensive/test_performance.cpp
    comprehensive/test_integration.cpp
)

target_link_libraries(nlm_comprehensive_test PRIVATE nlm_core)
target_compile_features(nlm_comprehensive_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME nlm_comprehensive_test COMMAND nlm_comprehensive_test)
