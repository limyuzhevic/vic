# tests/integration/CMakeLists.txt
# Integration tests CMake configuration

cmake_minimum_required(VERSION 3.16)

# Integration test executable
add_executable(nlm_integration_tests
    ../src/tests/integration/IntegrationTestFramework.cpp
    ../src/tests/integration/TestSystemIntegration.cpp
    ../src/tests/integration/TestMemoryIntegration.cpp
    ../src/tests/integration/TestCognitionIntegration.cpp
    ../src/tests/integration/TestNeuromodulationIntegration.cpp
    ../src/tests/integration/TestDevelopmentIntegration.cpp
    ../src/tests/integration/TestCrossSystemIntegration.cpp
)

target_link_libraries(nlm_integration_tests
    PUBLIC
    nlm_core
    nlm_agent
    nlm_world
    nlm_core_experiment
    nlm_brain_experiments
    nlm_memory_experiments
    nlm_cognition_experiments
    nlm_neuromodulation_experiments
)

target_include_directories(nlm_integration_tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

# Enable testing for integration tests
enable_testing()
add_test(NAME nlm_integration_tests COMMAND nlm_integration_tests)

# Integration test configuration
# These tests run independently of unit tests
# Each integration test can be run separately if needed
