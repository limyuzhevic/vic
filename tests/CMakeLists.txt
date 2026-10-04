cmake_minimum_required(VERSION 3.16)

# Find Python and pybind11 dependencies
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

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
    test_agent.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)

# Add agent library for tests that need it
target_link_libraries(nlm_test PRIVATE nlm_agent)

target_compile_features(nlm_test PRIVATE cxx_std_20)

# Python bindings for agent components
pybind11_add_module(pynlm_agent
    ../src/agent/pybind/AgentBrainPy.cpp
)
target_link_libraries(pynlm_agent PRIVATE nlm_agent pybind11::module)

# Set output name to match expected Python module name
set_target_properties(pynlm_agent PROPERTIES OUTPUT_NAME "pynlm_agent")

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)