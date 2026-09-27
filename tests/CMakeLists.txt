cmake_minimum_required(VERSION 3.16)

# Find GoogleTest
find_package(GTest REQUIRED)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/comprehensive)

# Create comprehensive test executable
add_executable(nlm_comprehensive_test
    tests/test_main.cpp
    tests/test_types.cpp
    tests/test_config.cpp
    tests/test_random.cpp
    tests/test_clock.cpp
    tests/test_neuron.cpp
    tests/test_synapse.cpp
    tests/test_stdp.cpp
    tests/test_brain.cpp
    comprehensive/test_main_comprehensive.cpp
    comprehensive/test_neuron_dynamics.cpp
    comprehensive/test_stdp.cpp
    comprehensive/test_hebbian.cpp
    comprehensive/test_working_memory.cpp
    comprehensive/test_episodic_memory.cpp
    comprehensive/test_attention.cpp
    comprehensive/test_development.cpp
    comprehensive/test_config_validation.cpp
)

target_link_libraries(nlm_comprehensive_test PRIVATE nlm_core GTest::gtest GTest::main)
target_compile_features(nlm_comprehensive_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_comprehensive_test COMMAND nlm_comprehensive_test)

# Benchmark executable (optional)
add_executable(nlm_benchmark
    comprehensive/benchmark.cpp
)
target_link_libraries(nlm_benchmark PRIVATE nlm_core)
target_compile_features(nlm_benchmark PRIVATE cxx_std_20)

# Add benchmark test
add_test(NAME nlm_benchmark COMMAND nlm_benchmark)
