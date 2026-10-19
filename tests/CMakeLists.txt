# Comprehensive test configuration for NLM
# This provides modern CMake testing infrastructure

cmake_minimum_required(VERSION 3.16)

# Import the main NLM targets
find_package(NLM REQUIRED)

# Enable testing framework
enable_testing()

# Test configuration
if(BUILD_TESTS)
    message(STATUS "Configuring tests for NLM")
    
    # Set up test directories
    set(TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(TEST_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    
    # Configure test executable with modern CMake
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
        test_integration.cpp
    )
    
    # Configure test properties
    target_link_libraries(nlm_test PRIVATE NLM::nlm_core)
    target_compile_features(nlm_test PRIVATE cxx_std_20)
    
    # Apply modern CMake compile options
    setup_compile_options(nlm_test)
    
    # Configure test properties
    target_include_directories(nlm_test PRIVATE
        ${TEST_SOURCE_DIR}/../src
        ${TEST_BINARY_DIR}
    )
    
    # Add test categories
    target_compile_definitions(nlm_test PRIVATE
        TEST_CATEGORY_CORE
        TEST_CATEGORY_BRAIN
        TEST_CATEGORY_DYNAMIC
    )
    
    # Add performance testing support
    if(NLM_ENABLE_PROFILING)
        target_compile_definitions(nlm_test PRIVATE NLM_PERF_TESTING=1)
        target_link_options(nlm_test PRIVATE -pg -g)
    endif()
    
    # Add coverage support
    if(NLM_ENABLE_COVERAGE)
        target_compile_options(nlm_test PRIVATE --coverage)
        target_link_options(nlm_test PRIVATE --coverage)
        target_link_libraries(nlm_test PRIVATE coverage)
    endif()
    
    # Add test
    add_test(NAME nlm_test
        COMMAND nlm_test
        WORKING_DIRECTORY ${TEST_BINARY_DIR}
    )
    
    # Set test properties
    set_tests_properties(nlm_test PROPERTIES
        TIMEOUT 300
        ENVIRONMENT "PATH=${TEST_BINARY_DIR}:$ENV{PATH}"
    )
    
    # Add test categories
    add_custom_target(unit_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS nlm_test
        COMMENT "Running unit tests"
    )
    
    # Add integration tests
    add_custom_target(integration_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS nlm_test
        COMMENT "Running integration tests"
    )
    
    # Add performance tests if enabled
    if(NLM_ENABLE_PROFILING)
        add_custom_target(performance_tests
            COMMAND ${CMAKE_COMMAND} -P ${TEST_BINARY_DIR}/performance_test.cmake
            DEPENDS nlm_test
            COMMENT "Running performance tests"
        )
    endif()
    
    # Add test data generation target
    add_custom_target(generate_test_data
        COMMAND ${CMAKE_COMMAND} -P ${TEST_BINARY_DIR}/generate_test_data.cmake
        COMMENT "Generating test data"
    )
    
    # Add test analysis target
    add_custom_target(analyze_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --output-junit ${TEST_BINARY_DIR}/test_results.xml
        COMMAND ${PYTHON_EXECUTABLE} ${TEST_BINARY_DIR}/analyze_tests.py
        DEPENDS nlm_test
        COMMENT "Analyzing test results"
    )
endif()

# Export test configuration
export(TARGETS nlm_test
    FILE "${TEST_BINARY_DIR}/TestTargets.cmake"
)

# Install test configuration
install(FILES "${TEST_BINARY_DIR}/TestTargets.cmake"
    DESTINATION lib/cmake/NLM
)
