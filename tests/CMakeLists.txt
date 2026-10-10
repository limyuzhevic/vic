cmake_minimum_required(VERSION 3.16)

# Find GoogleTest
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

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
    test_comprehensive.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core GTest::gtest GTest::gmock)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Install test executable (optional)
install(TARGETS nlm_test DESTINATION bin)

# Create additional test executables for specific components
add_executable(nlm_config_tests
    test_config.cpp
    test_comprehensive.cpp
)

target_link_libraries(nlm_config_tests PRIVATE nlm_core GTest::gtest)
target_compile_features(nlm_config_tests PRIVATE cxx_std_20)

add_test(NAME nlm_config_tests COMMAND nlm_config_tests)

add_executable(nlm_neuron_tests
    test_neuron.cpp
    test_comprehensive.cpp
)

target_link_libraries(nlm_neuron_tests PRIVATE nlm_core GTest::gtest)
target_compile_features(nlm_neuron_tests PRIVATE cxx_std_20)

add_test(NAME nlm_neuron_tests COMMAND nlm_neuron_tests)
