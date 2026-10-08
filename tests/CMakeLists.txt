cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create main test executable
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
    # Advanced integration tests
    test_integration.cpp
    test_experiments.cpp
    test_performance.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Performance benchmarks
add_executable(nlm_benchmark
    benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Phase 6 integration demo
add_executable(nlm_phase6_demo
    ../src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Enable testing
enable_testing()
add_test(NAME nlm_unit_tests COMMAND nlm_test)

# Add custom test targets for advanced testing
add_custom_target(integration_tests
    COMMAND ./nlm_test  # Run unit tests (includes integration)
    DEPENDS nlm_test
    COMMENT "Running all unit and integration tests"
)

add_custom_target(performance_tests
    COMMAND ./nlm_benchmark
    DEPENDS nlm_benchmark
    COMMENT "Running performance benchmarks"
)

add_custom_target(phase6_tests
    COMMAND ./nlm_phase6_demo
    DEPENDS nlm_phase6_demo
    COMMENT "Running Phase 6 integration demo"
)

add_custom_target(comprehensive_tests
    COMMAND echo "Running comprehensive test suite..."
    COMMAND ./nlm_test
    COMMAND ./nlm_benchmark
    COMMAND ./nlm_phase6_demo
    DEPENDS nlm_test nlm_benchmark nlm_phase6_demo
    COMMENT "Running comprehensive test suite (unit + integration + performance)"
)
