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
    test_hebbian.cpp
    test_plasticity_rule.cpp
    test_plasticity_suite.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Create benchmark executables
add_executable(benchmark_stdp
    benchmarks/benchmark_stdp.cpp
)
target_link_libraries(benchmark_stdp PRIVATE nlm_core)

add_executable(benchmark_hebbian
    benchmarks/benchmark_hebbian.cpp
)
target_link_libraries(benchmark_hebbian PRIVATE nlm_core)

add_executable(benchmark_plasticity_integration
    benchmarks/benchmark_plasticity_integration.cpp
)
target_link_libraries(benchmark_plasticity_integration PRIVATE nlm_core)

add_executable(benchmark_suite
    benchmarks/benchmark_suite.cpp
)
target_link_libraries(benchmark_suite PRIVATE nlm_core)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
