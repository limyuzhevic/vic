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
    test_advanced.cpp
)

target_link_libraries(nlm_test PUBLIC nlm_core)
set_target_properties(nlm_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED TRUE)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
