cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create enhanced test executable
add_executable(nlm_enhanced_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_all_enhanced.cpp
)

target_link_libraries(nlm_enhanced_test PRIVATE nlm_core)
target_compile_features(nlm_enhanced_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_enhanced_test COMMAND nlm_enhanced_test)
