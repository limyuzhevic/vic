cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create comprehensive test executable
add_executable(nlm_test_comprehensive
    test_main.cpp
    test_main_integration.cpp
    test_framework.hpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

# Add integration test source files
# Note: The actual implementation would be in separate .cpp files
# For now, the integration tests are in test_main_integration.cpp

target_link_libraries(nlm_test_comprehensive PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test_comprehensive PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test_comprehensive COMMAND nlm_test_comprehensive)

# Additional test targets for specific phases

# Phase 2 specific tests
add_executable(nlm_test_phase2
    test_main.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)
target_link_libraries(nlm_test_phase2 PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test_phase2 PRIVATE cxx_std_20)
add_test(NAME nlm_test_phase2 COMMAND nlm_test_phase2)

# Phase 3 specific tests
add_executable(nlm_test_phase3
    test_main.cpp
    test_brain.cpp
)
# Add world and agent test files when available
target_link_libraries(nlm_test_phase3 PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(nlm_test_phase3 PRIVATE cxx_std_20)
add_test(NAME nlm_test_phase3 COMMAND nlm_test_phase3)

# Phase 4 specific tests
add_executable(nlm_test_phase4
    test_main.cpp
    test_brain.cpp
)
target_link_libraries(nlm_test_phase4 PRIVATE nlm_core nlm_agent nlm_phase3)
target_compile_features(nlm_test_phase4 PRIVATE cxx_std_20)
add_test(NAME nlm_test_phase4 COMMAND nlm_test_phase4)

# Python bindings test target (when Python bindings are available)
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    add_executable(nlm_test_python_bindings
        test_main.cpp
        test_config.cpp
    )
    target_link_libraries(nlm_test_python_bindings PRIVATE nlm_core nlm_agent pynlm)
    target_compile_features(nlm_test_python_bindings PRIVATE cxx_std_20)
    add_test(NAME nlm_test_python_bindings COMMAND nlm_test_python_bindings)
endif()
