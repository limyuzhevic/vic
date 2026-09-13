cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create comprehensive test suite executable
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

# Enable testing with CTest integration
enable_testing()
add_test(NAME nlm_unit_tests COMMAND nlm_test)

# Add test configuration files and data
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.ini.in
    ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
    @ONLY
)

# Set up test environment variables
set(TEST_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ENV{TEST_CONFIG_PATH} ${TEST_CONFIG_DIR}/config.ini)

# Create test data directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_data)

# Configure test data generation script
add_custom_target(generate_test_data
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_test_data.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating test data for NLM tests"
    VERBATIM
)
