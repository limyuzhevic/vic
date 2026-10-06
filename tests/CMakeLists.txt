cmake_minimum_required(VERSION 3.16)

project(NLMTests LANGUAGES CXX)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src")

# Core library (re-use existing)
add_library(nlm_core ALIAS nlm_core)

# Test executables

# Config tests
add_executable(test_config
    test_config.cpp
)
target_link_libraries(test_config PUBLIC nlm_core)

# Config integration tests
add_executable(test_config_integration
    test_config_integration.cpp
)
target_link_libraries(test_config_integration PUBLIC nlm_core)

# Demo executable for configuration improvements
add_executable(config_demo
    ../../demo/config_demo.cpp
)
target_link_libraries(config_demo PUBLIC nlm_core)

# Test all targets
add_test(NAME test_config COMMAND test_config)
add_test(NAME test_config_integration COMMAND test_config_integration)
add_test(NAME config_demo COMMAND config_demo)