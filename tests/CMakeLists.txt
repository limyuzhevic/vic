cmake_minimum_required(VERSION 3.16)

# Find GoogleTest
find_package(GTest REQUIRED)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${GTEST_INCLUDE_DIRS})

# Create test executable using Google Test
add_executable(nlm_test
    test_neuron.cpp
    test_stdp.cpp
    test_config.cpp
    test_synapse.cpp
    test_types.cpp
    test_clock.cpp
    test_brain.cpp
    test_python_bindings.cpp
    test_implementation_status.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core GTest::gtest GTest::gtest_main)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Python bindings test executable
add_executable(test_python_bindings
    test_python_bindings.cpp
)

target_link_libraries(test_python_bindings PRIVATE nlm_core GTest::gtest GTest::gtest_main)
target_compile_features(test_python_bindings PRIVATE cxx_std_20)

# Enable Python test
add_test(NAME test_python_bindings COMMAND test_python_bindings)

# Implementation status tracker test executable
add_executable(nlm_status_tracker_test
    test_implementation_status.cpp
)

target_link_libraries(nlm_status_tracker_test PRIVATE nlm_core GTest::gtest GTest::gtest_main)
target_compile_features(nlm_status_tracker_test PRIVATE cxx_std_20)

# Enable status tracker test
add_test(NAME nlm_status_tracker_test COMMAND nlm_status_tracker_test)
