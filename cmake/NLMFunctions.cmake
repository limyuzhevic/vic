# NLM CMake functions and utilities
# This module provides helper functions for the NLM build system

include(CMakeParseArguments)

# Function to create a test that uses NLM
function(nlm_add_test test_name source_file)
    if (NOT TARGET ${test_name})
        add_executable(${test_name} ${source_file})
        target_link_libraries(${test_name} PRIVATE nlm::core)
        add_test(NAME ${test_name} COMMAND ${test_name})
        
        # Add test-specific compile options
        target_compile_definitions(${test_name} PRIVATE TESTING=1)
        
        # Add test output directory
        set_target_properties(${test_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
    endif()
endfunction()

# Function to create a NLM example application
function(nlm_add_example example_name source_file)
    if (NOT TARGET ${example_name})
        add_executable(${example_name} ${source_file})
        target_link_libraries(${example_name} PRIVATE nlm::core)
        
        # Add example output directory
        set_target_properties(${example_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
        )
    endif()
endfunction()

# Function to create a NLM plugin
function(nlm_add_plugin plugin_name source_file)
    if (NOT TARGET ${plugin_name})
        add_library(${plugin_name} SHARED ${source_file})
        target_link_libraries(${plugin_name} PRIVATE nlm::core)
        
        # Set plugin properties
        set_target_properties(${plugin_name} PROPERTIES
            PREFIX ""
            SUFFIX ".nlmplugin"
            POSITION_INDEPENDENT_CODE ON
        )
    endif()
endfunction()

# Function to set up a test environment
function(nlm_setup_test_environment)
    # Set up test-specific directories
    set(TEST_DATA_DIR ${CMAKE_SOURCE_DIR}/tests/data CACHE PATH "Test data directory")
    set(TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/test_output CACHE PATH "Test output directory")
    
    # Create directories if they don't exist
    file(MAKE_DIRECTORY ${TEST_DATA_DIR})
    file(MAKE_DIRECTORY ${TEST_OUTPUT_DIR})
    
    # Add to path
    set(ENV{PATH} "${TEST_OUTPUT_DIR}:${ENV{PATH}}")
endfunction()

# Function to check if NLM is configured with specific features
function(nlm_check_features REQUIRED_FEATURES OPTIONAL_FEATURES)
    set(${REQUIRED_FEATURES}_FOUND TRUE)
    set(${OPTIONAL_FEATURES}_FOUND FALSE)
    
    # Check for required features
    foreach(feature ${${REQUIRED_FEATURES}})
        if (NOT NLM_HAS_${feature})
            message(FATAL_ERROR "Required feature ${feature} not enabled")
        endif()
    endforeach()
    
    # Check for optional features
    foreach(feature ${${OPTIONAL_FEATURES}})
        if (NLM_HAS_${feature})
            set(${OPTIONAL_FEATURES}_FOUND TRUE CACHE BOOL "Found optional feature ${feature}")
        endif()
    endforeach()
endfunction()
