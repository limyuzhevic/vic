cmake_minimum_required(VERSION 3.16

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create main test executable (original tests)
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

target_link_libraries(nlm_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Create enhanced test executable for new configuration functionality
add_executable(nlm_test_enhanced
    test_config_enhanced.cpp
)

target_link_libraries(nlm_test_enhanced PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test_enhanced PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME nlm_test_enhanced COMMAND nlm_test_enhanced)