cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create main test executable with enhanced framework
add_executable(nlm_test
    test_main.cpp
    test_enhanced.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Performance benchmark test
set(BENCHMARK_SOURCES
    ../src/benchmark.cpp
)

add_executable(nlm_benchmark ${BENCHMARK_SOURCES})
target_link_libraries(nlm_benchmark PRIVATE nlm_core)
target_compile_features(nlm_benchmark PRIVATE cxx_std_20)

# Phase 2 demo test (original)
set(PHASE2_SOURCES
    ../src/main.cpp
)

add_executable(nlm_demo ${PHASE2_SOURCES})
target_link_libraries(nlm_demo PRIVATE nlm_core)
target_compile_features(nlm_demo PRIVATE cxx_std_20)

# Enhanced demo test with new CLI
target_link_libraries(nlm_demo PRIVATE nlm_core)
target_compile_features(nlm_demo PRIVATE cxx_std_20)

# Install tests
install(TARGETS nlm_test nlm_benchmark nlm_demo
    RUNTIME DESTINATION bin
)
