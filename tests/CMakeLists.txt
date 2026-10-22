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
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Create Phase 4 integration test executable
add_executable(nlm_phase4_integration_tests
    test_phase4_integration.cpp
    test_config.cpp  # Include config tests for setup
    test_brain.cpp   # Include brain tests for setup
)

target_link_libraries(nlm_phase4_integration_tests PRIVATE nlm_core nlm_phase4)
target_compile_features(nlm_phase4_integration_tests PRIVATE cxx_std_20)

# Create Phase 6 integration test executable
add_executable(nlm_phase6_integration_tests
    test_phase6_integration.cpp
    test_config.cpp  # Include config tests for setup
    test_brain.cpp   # Include brain tests for setup
    nlm_agent
    nlm_world
)

target_link_libraries(nlm_phase6_integration_tests PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_features(nlm_phase6_integration_tests PRIVATE cxx_std_20)

# Enable testing
enable_testing()

add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME nlm_phase4_integration_tests COMMAND nlm_phase4_integration_tests)
add_test(NAME nlm_phase6_integration_tests COMMAND nlm_phase6_integration_tests)
