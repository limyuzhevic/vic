cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable
add_executable(nlm_test
    tests/test_main.cpp
    tests/test_types.cpp
    tests/test_config.cpp
    tests/test_random.cpp
    tests/test_clock.cpp
    tests/test_neuron.cpp
    tests/test_synapse.cpp
    tests/test_stdp.cpp
    tests/test_brain.cpp
    tests/test_phase6_integration.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
