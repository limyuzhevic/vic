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
    test_brain.cpp
    test_neuromodulation.cpp
    test_memory.cpp
    test_development.cpp
    test_prediction.cpp
    test_cognition.cpp
    test_sensory.cpp
    test_motor.cpp
    test_environment.cpp
    test_world.cpp
    test_visualization.cpp
    test_performance.cpp
    test_experiment.cpp
    test_cli.cpp
    test_agent.cpp
    test_plasticity.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
