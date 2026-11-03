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
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Add Python bindings test target (Phase 6 enhancement)
# This tests the enhanced Python bindings with JSON/YAML configuration
if(PYTHON_EXECUTABLE)
    add_custom_target(python_bindings_test
        COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_python_bindings.py -v
        DEPENDS nlm_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python bindings tests..."
        VERBATIM
    )
endif()

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add Python bindings test to the test suite
if(PYTHON_EXECUTABLE)
    add_test(NAME python_bindings_test COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_python_bindings.py -v)
endif()
