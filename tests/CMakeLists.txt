cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable with enhanced tests
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
    test_enhanced_suite.cpp
)

target_link_libraries(nlm_test_enhanced PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test_enhanced PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test_enhanced COMMAND nlm_test_enhanced)

# Add benchmark tests
add_executable(nlm_benchmark_test
    ../src/benchmark.cpp
)
target_link_libraries(nlm_benchmark_test PRIVATE nlm_core)
add_test(NAME nlm_benchmark_test COMMAND nlm_benchmark_test --benchmark-mode)

# Add demo tests
add_executable(nlm_phase6_demo_test
    ../src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase6)
add_test(NAME nlm_phase6_demo_test COMMAND nlm_phase6_demo_test --neurons 10 --steps 10 --no-progress)

# Legacy test executable (for backward compatibility)
add_executable(nlm_legacy_test
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
target_link_libraries(nlm_legacy_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_legacy_test PRIVATE cxx_std_20)
add_test(NAME nlm_legacy_test COMMAND nlm_legacy_test)
