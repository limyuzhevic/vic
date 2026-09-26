cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create main C++ test executable
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

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Create Python integration test executable
add_executable(nlm_python_test
    test_python_main.cpp
    test_python_bindings.cpp
    test_enhanced_features.cpp
)

target_link_libraries(nlm_python_test PRIVATE nlm_core)
target_compile_features(nlm_python_test PRIVATE cxx_std_20)

# Add Python test to test suite
add_test(NAME nlm_python_test COMMAND nlm_python_test)
