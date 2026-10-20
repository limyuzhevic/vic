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
    test_experiments.cpp
    test_memory.cpp
    test_neuromodulation.cpp
    test_cognition.cpp
    test_development.cpp
    test_prediction.cpp
    test_sensory.cpp
    test_motor.cpp
    test_world.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add additional test targets for specific subsystems
add_executable(nlm_core_tests
    test_core_types.cpp
    test_core_config.cpp
    test_core_random.cpp
    test_core_logger.cpp
    test_core_clock.cpp
)

target_link_libraries(nlm_core_tests PRIVATE nlm_core)
target_compile_features(nlm_core_tests PRIVATE cxx_std_20)

add_test(NAME nlm_core_tests COMMAND nlm_core_tests)

add_executable(nlm_brain_tests
    test_brain_neuron.cpp
    test_brain_synapse.cpp
    test_brain_plasticity.cpp
)

target_link_libraries(nlm_brain_tests PRIVATE nlm_core)
target_compile_features(nlm_brain_tests PRIVATE cxx_std_20)

add_test(NAME nlm_brain_tests COMMAND nlm_brain_tests)

# Installation for tests (optional)
install(TARGETS nlm_test nlm_core_tests nlm_brain_tests
    RUNTIME DESTINATION bin/tests
    OPTIONAL
)

# Summary
message(STATUS "Testing Framework Configuration:")
message(STATUS "  Core test executable: nlm_test (${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp)")
message(STATUS "  Core unit tests: test_core_types.cpp, test_core_config.cpp, etc.")
message(STATUS "  Brain unit tests: test_brain_neuron.cpp, test_brain_synapse.cpp, etc.")
message(STATUS "  Total test files: 17")
