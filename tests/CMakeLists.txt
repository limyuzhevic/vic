# Modern CMake tests configuration for NLM
# Uses target-based approach with proper dependency management

cmake_minimum_required(VERSION 3.16)

# Import modern CMake utilities
include(../cmake/NLMConfig.cmake)

# Test configuration validation
function(validate_tests_config)
    # Check for test sources
    set(TEST_SOURCES
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
    
    foreach(test_source IN LISTS TEST_SOURCES)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_source}")
            message(WARNING "Test source ${test_source} not found")
        endif()
    endforeach()
endfunction()

validate_tests_config()

# Modern test target using target-based approach
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

# Configure test target with modern CMake
target_compile_features(nlm_test PRIVATE cxx_std_20)
target_link_libraries(nlm_test PRIVATE nlm_core)

# Include directories (modern target-based approach)
target_include_directories(nlm_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Types
)

# Compiler-specific test configurations
if(MSVC)
    target_compile_options(nlm_test PRIVATE /WX)  # Treat warnings as errors
else()
    target_compile_options(nlm_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Test configuration for modern testing frameworks
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(nlm_test PRIVATE NLM_DEBUG_TESTS)
endif()

# Add test to CTest with metadata
add_test(NAME nlm_unit_tests COMMAND nlm_test)
set_tests_properties(nlm_unit_tests PROPERTIES
    TIMEOUT 300
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Generate test reports (modern CMake)
if(BUILD_PERFORMANCE_TESTS)
    add_custom_command(TARGET nlm_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Unit tests completed successfully"
        COMMENT "Generating test reports"
        VERBATIM
    )
endif()

# Add coverage support for modern C++ testing
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND_coverage)
    target_compile_options(nlm_test PRIVATE --coverage)
    target_link_libraries(nlm_test PRIVATE --coverage)
endif()

message(STATUS "Modern testing infrastructure configured for NLM")
