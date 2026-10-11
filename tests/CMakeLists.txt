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
    test_python_bindings.cpp
    benchmark_python.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add Python API test script
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/test_python_api.py
    ${CMAKE_CURRENT_BINARY_DIR}/test_python_api.py
    COPYONLY
)
add_custom_target(python_api_tests
    COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/test_python_api.py
    DEPENDS nlm_test
    COMMENT "Running Python API tests"
)
