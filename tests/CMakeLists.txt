cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create comprehensive test executable
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
    test_performance.cpp
    test_integration.cpp
    test_memory.cpp
    test_api.cpp
    src/test_convenience.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Create separate test executables for specific test suites

# Performance benchmark executable
add_executable(benchmark_test
    tests/benchmark_main.cpp
)
target_link_libraries(benchmark_test PRIVATE nlm_core)
target_compile_features(benchmark_test PRIVATE cxx_std_20)
add_test(NAME benchmark_test COMMAND benchmark_test)

# Integration test executable
add_executable(integration_test
    tests/integration_main.cpp
)
target_link_libraries(integration_test PRIVATE nlm_core)
target_compile_features(integration_test PRIVATE cxx_std_20)
add_test(NAME integration_test COMMAND integration_test)

# Memory management test executable
add_executable(memory_test
    tests/memory_main.cpp
)
target_link_libraries(memory_test PRIVATE nlm_core)
target_compile_features(memory_test PRIVATE cxx_std_20)
add_test(NAME memory_test COMMAND memory_test)

# API surface test executable
add_executable(api_test
    tests/api_main.cpp
)
target_link_libraries(api_test PRIVATE nlm_core)
target_compile_features(api_test PRIVATE cxx_std_20)
add_test(NAME api_test COMMAND api_test)

# Convenience functions test executable
add_executable(convenience_test
    tests/convenience_main.cpp
)
target_link_libraries(convenience_test PRIVATE nlm_core)
target_compile_features(convenience_test PRIVATE cxx_std_20)
add_test(NAME convenience_test COMMAND convenience_test)
