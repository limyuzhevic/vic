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
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Python bindings test requires pybind11
find_package(pybind11 REQUIRED)
# Check if pybind11 is available for Python bindings tests
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import pybind11"
    RESULT_VARIABLE PYBIND11_AVAILABLE
    OUTPUT_QUIET
    ERROR_QUIET
)

if(PYBIND11_AVAILABLE EQUAL 0)
    # Python bindings test is enabled
    include_directories(${PYTHON_INCLUDE_DIRS})
    target_link_libraries(nlm_test PRIVATE pybind11::module)
    add_test(NAME nlm_python_bindings_test COMMAND nlm_test)
else()
    message(STATUS "Python bindings tests skipped (pybind11 not available)")
endif()

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
