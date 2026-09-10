cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create comprehensive test executable with all tests
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
    test_brain_safety.cpp
    test_config_safety.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Add Python bindings tests
if(PYTHON_EXECUTABLE)
    find_package(pybind11 REQUIRED)
    pybind11_add_module(pynlm_tests test_python_bindings.cpp)
    target_link_libraries(pynlm_tests PRIVATE nlm_core pybind11::module)
endif()

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add Python module tests if built
if(PYTHON_EXECUTABLE)
    add_test(NAME pynlm_tests COMMAND $<TARGET_FILE:pynlm_tests>)
endif()
