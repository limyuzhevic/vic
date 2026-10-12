# Integration test CMake configuration
# 
# CMake configuration for NLM cross-system integration tests
# Includes all integration test directories and configuration files

cmake_minimum_required(VERSION 3.16)

# Integration test directories
file(GLOB INTEGRATION_TEST_HEADERS
    "integration/*.hpp"
)

file(GLOB INTEGRATION_TEST_SOURCES
    "integration/*.cpp"
)

file(GLOB TEST_DATA_FILES
    "data/*.json"
    "data/*.hpp"
)

# Integration test executable
add_executable(nlm_integration_test
    ${INTEGRATION_TEST_SOURCES}
)

target_include_directories(nlm_integration_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${NLM_SOURCE_DIR}
)

target_link_libraries(nlm_integration_test PRIVATE nlm_core)

target_compile_features(nlm_integration_test PRIVATE cxx_std_20)

# Enable testing for integration tests
enable_testing()
add_test(NAME nlm_integration_test COMMAND nlm_integration_test)

# Integration test configuration files
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/data/integration_test_config.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/integration_test_config.hpp"
    @ONLY
)

# Copy test data files to build directory
add_custom_target(copy_test_data ALL
    COMMENT "Copying integration test data files"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/test_data
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/data/brain_states.json
        ${CMAKE_CURRENT_BINARY_DIR}/test_data/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/data/experiment_scenarios.json
        ${CMAKE_CURRENT_BINARY_DIR}/test_data/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/data/integration_test_cases.json
        ${CMAKE_CURRENT_BINARY_DIR}/test_data/
    DEPENDS nlm_integration_test
)