cmake_minimum_required(VERSION 3.16)

# Test configuration - use modern CMake target-based includes
include(CTest)
include(CMakeParseArguments)

# Forward declaration for test function
function(nlm_add_test_executable test_name test_sources)
    add_executable(${test_name} ${test_sources})
    target_compile_features(${test_name} PRIVATE cxx_std_20)
    nlm_add_test(${test_name})
endfunction()

function(nlm_add_test test_name)
    if(TARGET ${test_name})
        add_test(NAME ${test_name} COMMAND ${test_name})
        set_tests_properties(${test_name} PROPERTIES LABELS "nlm;unit_test")
    endif()
endfunction()

# Test discovery - collect all test source files
file(GLOB_RECURSE ALL_TEST_SOURCES "*.cpp")

# Extract core module tests (based on source file patterns)
list(FILTER CORE_TEST_SOURCES INCLUDE_REGEXP "test_(types|config|random|clock|neuron|synapse|stdp|brain)\\.cpp")

# Extract application module tests
list(FILTER APPLICATION_TEST_SOURCES INCLUDE_REGEXP "test_(commandline_parser|configuration_loader|experiment_runner|brain_initializer|user_interface|performance_profiler|basic_connectivity|plasticity_experiment|stdp_verification)\\.cpp")

# Core functionality tests
add_executable(nlm_core_test
    src/core_test_main.cpp
    ${CORE_TEST_SOURCES}
)

target_link_libraries(nlm_core_test PRIVATE nlm_core)
nlm_add_test(nlm_core_test)

# Application module tests
add_executable(nlm_application_test
    tests/application_test_main.cpp
    ${APPLICATION_TEST_SOURCES}
)

target_link_libraries(nlm_application_test PRIVATE nlm_core nlm_application)
nlm_add_test(nlm_application_test)

# Agent system tests
add_executable(nlm_agent_test
    src/agent_test_main.cpp
)

target_link_libraries(nlm_agent_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_agent_test PRIVATE cxx_std_20)
nlm_add_test(nlm_agent_test)

# World system tests
add_executable(nlm_world_test
    src/world_test_main.cpp
)

target_link_libraries(nlm_world_test PRIVATE nlm_core nlm_world)
target_compile_features(nlm_world_test PRIVATE cxx_std_20)
nlm_add_test(nlm_world_test)

# Phase 3 experiment tests
add_executable(nlm_phase3_test
    src/phase3_test_main.cpp
)

target_link_libraries(nlm_phase3_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3)
target_compile_features(nlm_phase3_test PRIVATE cxx_std_20)
nlm_add_test(nlm_phase3_test)

# Phase 4 experiment tests
add_executable(nlm_phase4_test
    src/phase4_test_main.cpp
)

target_link_libraries(nlm_phase4_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_features(nlm_phase4_test PRIVATE cxx_std_20)
nlm_add_test(nlm_phase4_test)

# Enable testing with CTest
enable_testing()

# Add test labels for filtering
add_test(NAME nlm_core_test COMMAND nlm_core_test)
add_test(NAME nlm_application_test COMMAND nlm_application_test)
add_test(NAME nlm_agent_test COMMAND nlm_agent_test)
add_test(NAME nlm_world_test COMMAND nlm_world_test)
add_test(NAME nlm_phase3_test COMMAND nlm_phase3_test)
add_test(NAME nlm_phase4_test COMMAND nlm_phase4_test)

# Set up test properties
set_tests_properties(nlm_core_test nlm_application_test nlm_agent_test nlm_world_test nlm_phase3_test nlm_phase4_test
    PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    TIMEOUT 300
)
