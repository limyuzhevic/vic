cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Core Component Tests
add_executable(test_configmanager test_configmanager.cpp)
target_link_libraries(test_configmanager nlm_core)
add_test(NAME test_configmanager COMMAND test_configmanager)

add_executable(test_commandlineparser test_commandlineparser.cpp)
target_link_libraries(test_commandlineparser nlm_core)
add_test(NAME test_commandlineparser COMMAND test_commandlineparser)

add_executable(test_testrunner test_testrunner.cpp)
target_link_libraries(test_testrunner nlm_core)
add_test(NAME test_testrunner COMMAND test_testrunner)

add_executable(test_outputformatter test_outputformatter.cpp)
target_link_libraries(test_outputformatter nlm_core nlm_agent)
add_test(NAME test_outputformatter COMMAND test_outputformatter)

# Integration and Performance Tests
add_executable(test_integration test_integration.cpp)
target_link_libraries(test_integration nlm_core nlm_agent nlm_world)
add_test(NAME test_integration COMMAND test_integration)

add_executable(test_performance test_performance.cpp)
target_link_libraries(test_performance nlm_core nlm_agent nlm_world)
add_test(NAME test_performance COMMAND test_performance)

add_executable(test_backward_compatibility test_backward_compatibility.cpp)
target_link_libraries(test_backward_compatibility nlm_core nlm_agent nlm_world)
add_test(NAME test_backward_compatibility COMMAND test_backward_compatibility)

# Original tests (for backward compatibility)
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
