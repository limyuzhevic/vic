cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create main test executable (basic unit tests)
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

# Enable testing for main tests
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Integration test executable
add_executable(nlm_integration_test
    integration_test_main.cpp
)

target_link_libraries(nlm_integration_test PRIVATE nlm_core)
target_compile_features(nlm_integration_test PRIVATE cxx_std_20)

# Enable integration testing
add_test(NAME nlm_integration_test COMMAND nlm_integration_test)

# Performance test executable
add_executable(nlm_performance_test
    performance_test_main.cpp
)

target_link_libraries(nlm_performance_test PRIVATE nlm_core)
target_compile_features(nlm_performance_test PRIVATE cxx_std_20)

# Enable performance testing
add_test(NAME nlm_performance_test COMMAND nlm_performance_test)
