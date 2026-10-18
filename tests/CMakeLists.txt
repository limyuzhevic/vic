cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create enhanced test executable
add_executable(nlm_test_enhanced
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_enhanced_framework.cpp
)

target_link_libraries(nlm_test_enhanced PRIVATE nlm_core)
target_compile_features(nlm_test_enhanced PRIVATE cxx_std_20)

# Create legacy test executable (for backward compatibility)
add_executable(nlm_test_legacy
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

target_link_libraries(nlm_test_legacy PRIVATE nlm_core)
target_compile_features(nlm_test_legacy PRIVATE cxx_std_20)

# Enable testing
enable_testing()

# Add both test executables
add_test(NAME nlm_test_enhanced COMMAND nlm_test_enhanced)
add_test(NAME nlm_test_legacy COMMAND nlm_test_legacy)
