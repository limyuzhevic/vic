cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create comprehensive test executable
add_executable(nlm_comprehensive_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_comprehensive.cpp
)

# Create integration test executable for Phase 6
add_executable(nlm_phase6_test
    src/experiments/Phase6Demo.cpp
)

target_link_libraries(nlm_comprehensive_test PRIVATE nlm_core)
target_link_libraries(nlm_phase6_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

target_compile_features(nlm_comprehensive_test PRIVATE cxx_std_20)
target_compile_features(nlm_phase6_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_comprehensive_test COMMAND nlm_comprehensive_test)
add_test(NAME nlm_phase6_test COMMAND nlm_phase6_test)

# Performance benchmark test
add_executable(nlm_benchmark
    src/benchmark.cpp
)

target_link_libraries(nlm_benchmark PRIVATE nlm_core)
target_compile_features(nlm_benchmark PRIVATE cxx_std_20)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_test(NAME nlm_benchmark COMMAND nlm_benchmark)
endif()
