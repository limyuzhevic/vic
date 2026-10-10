cmake_minimum_required(VERSION 3.19)

# Test configuration - standardized include paths using relative paths
set(NLM_TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
include_directories(${NLM_TEST_SOURCE_DIR})

# Comprehensive warning levels for tests
function(setup_test_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target_name} PRIVATE -Wshadow -Wnon-virtual-dtor -Wold-style-cast)
        endif()
    endif()
endfunction()

# Create test executable with standardized paths and comprehensive warnings
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
    test_framework_utils.cpp
    test_dopamine.cpp
    test_curiosity.cpp
    test_novelty.cpp
    test_prediction_error.cpp
    test_neural_planner.cpp
    test_concept_formation.cpp
    test_attentional_selection.cpp
    test_agent_brain.cpp
    test_sensory_percept.cpp
    test_visualization_interface.cpp
    test_development_system.cpp
    test_error_conditions.cpp
    test_performance.cpp
    test_integration.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Apply warning levels
setup_test_warnings(nlm_test)

# Test coverage configuration if enabled
if(NLM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_test PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(nlm_test PRIVATE --coverage)
endif()

# Sanitizer configurations for tests
if(NLM_ENABLE_ASAN AND NOT MSVC)
    target_compile_options(nlm_test PRIVATE -fsanitize=address)
    target_link_options(nlm_test PRIVATE -fsanitize=address)
endif()

if(NLM_ENABLE_TSAN AND NOT MSVC)
    target_compile_options(nlm_test PRIVATE -fsanitize=thread)
    target_link_options(nlm_test PRIVATE -fsanitize=thread)
endif()

# Enable testing with enhanced configuration
enable_testing()

# Add test cases with proper integration
add_test(NAME nlm_test
    COMMAND nlm_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Set test timeout for CI/CD environments
set_tests_properties(nlm_test
    PROPERTY TIMEOUT 300
)
