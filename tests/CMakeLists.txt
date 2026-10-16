cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable
add_executable(nlm_test
    test_main_enhanced.cpp
    test_types_enhanced.cpp
    test_config_enhanced.cpp
    test_random_enhanced.cpp
    test_clock_enhanced.cpp
    test_neuron_enhanced.cpp
    test_synapse_enhanced.cpp
    test_stdp_enhanced.cpp
    test_brain_enhanced.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
