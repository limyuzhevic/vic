cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(/usr/include/gtest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/unit)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../fixtures)

# Find GoogleTest
find_package(GTest REQUIRED)

# Create test executable
add_executable(nlm_test
    test_main_google.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    # New GoogleTest-based tests
    unit/test_config_validator.cpp
    unit/test_neuron_dynamics.cpp
    unit/test_synapse_plasticity.cpp
    unit/test_error_handling.cpp
    unit/test_performance_monitoring.cpp
    integration/test_integration.cpp
    python_bindings/test_python_bindings.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core GTest::gtest GTest::gtest_main)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Coverage support (if available)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_test PRIVATE --coverage)
    target_link_libraries(nlm_test PRIVATE --coverage)
endif()
