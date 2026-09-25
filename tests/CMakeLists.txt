cmake_minimum_required(VERSION 3.16)

# Find GoogleTest
find_package(GTest REQUIRED)
find_package Threads REQUIRED)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Unit tests executable
add_executable(nlm_unit_tests
    test_main.cpp
    common/test_base.cpp
    unit/test_neuron.cpp
    unit/test_synapse.cpp
    unit/test_plasticity.cpp
    unit/test_memory.cpp
    unit/test_brain.cpp
    unit/test_config.cpp
    unit/test_clock.cpp
    unit/test_random.cpp
    unit/test_types.cpp
)

target_link_libraries(nlm_unit_tests
    PRIVATE
        nlm_core
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

target_compile_features(nlm_unit_tests PRIVATE cxx_std_20)

# Add unit tests target
add_test(NAME nlm_unit_tests COMMAND nlm_unit_tests)

# Integration tests executable
add_executable(nlm_integration_tests
    integration/test_integration.cpp
)

target_link_libraries(nlm_integration_tests
    PRIVATE
        nlm_core
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

target_compile_features(nlm_integration_tests PRIVATE cxx_std_20)

# Add integration tests target
add_test(NAME nlm_integration_tests COMMAND nlm_integration_tests)

# Performance tests executable
add_executable(nlm_performance_tests
    performance/test_performance.cpp
)

target_link_libraries(nlm_performance_tests
    PRIVATE
        nlm_core
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

target_compile_features(nlm_performance_tests PRIVATE cxx_std_20)

# Add performance tests target
add_test(NAME nlm_performance_tests COMMAND nlm_performance_tests)

# Common utilities executable
add_executable(nlm_common_tests
    common/test_base.cpp
    common/test_macros.cpp
)

target_link_libraries(nlm_common_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

target_compile_features(nlm_common_tests PRIVATE cxx_std_20)

# Enhanced configuration system tests
add_executable(nlm_enhanced_config_tests
    test_enhanced_config_system.cpp
)

target_link_libraries(nlm_enhanced_config_tests
    PRIVATE
        nlm_core
        GTest::gtest
        GTest::gtest_main
)

target_compile_features(nlm_enhanced_config_tests PRIVATE cxx_std_20)

# Add enhanced config tests target
add_test(NAME nlm_enhanced_config_tests COMMAND nlm_enhanced_config_tests)

# Comprehensive test suite target
add_custom_target(comprehensive_tests ALL
    DEPENDS nlm_unit_tests nlm_integration_tests nlm_performance_tests nlm_enhanced_config_tests
    COMMENT "Building comprehensive NLM test suite"
)

# Set up test properties
function(setup_test_properties target_name)
    set_tests_properties(${target_name}
        PROPERTIES
            TIMEOUT 300
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            LABELS "tests"
    )
endfunction()

# Apply test properties to all test targets
set(TEST_TARGETS nlm_unit_tests nlm_integration_tests nlm_performance_tests nlm_enhanced_config_tests)
foreach(test_target ${TEST_TARGETS})
    if(TARGET ${test_target})
        setup_test_properties(${test_target})
    endif()
endforeach()

# Create test summary target
add_custom_target(test_summary
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS comprehensive_tests
    COMMENT "Running comprehensive NLM test suite with detailed output"
)

# Add test configuration options
option(BUILD_UNIT_TESTS "Build unit tests" ON)
option(BUILD_INTEGRATION_TESTS "Build integration tests" ON)
option(BUILD_PERFORMANCE_TESTS "Build performance tests" ON)
option(BUILD_ENHANCED_CONFIG_TESTS "Build enhanced config system tests" ON)

# Conditionally build tests based on options
if(BUILD_UNIT_TESTS)
    add_dependencies(comprehensive_tests nlm_unit_tests)
endif()

if(BUILD_INTEGRATION_TESTS)
    add_dependencies(comprehensive_tests nlm_integration_tests)
endif()

if(BUILD_PERFORMANCE_TESTS)
    add_dependencies(comprehensive_tests nlm_performance_tests)
endif()

if(BUILD_ENHANCED_CONFIG_TESTS)
    add_dependencies(comprehensive_tests nlm_enhanced_config_tests)
endif()

# Set up test discovery
enable_testing()

# Add custom test command
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS comprehensive_tests
    COMMENT "Running all NLM tests with verbose output"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)