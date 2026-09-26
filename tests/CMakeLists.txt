cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable with Phase 6 integration tests
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
    test_memory.cpp
    test_neuromodulation.cpp
    test_prediction.cpp
    test_cognition.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add Phase demo tests (these are integration tests)
add_test(NAME nlm_test_phase3 COMMAND nlm_phase3_demo)
add_test(NAME nlm_test_phase4 COMMAND nlm_phase4_demo)
add_test(NAME nlm_test_phase5 COMMAND nlm_phase5_demo)
add_test(NAME nlm_test_phase6 COMMAND nlm_phase6_demo)
