cmake_minimum_required(VERSION 3.16)

# Testing framework setup for NLM project
include(FetchContent)

# Find and configure Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Test target with modern CMake patterns
add_executable(nlm_test_all
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

target_link_libraries(nlm_test_all PRIVATE
    nlm_core
    GTest::gtest
    GTest::gtest_main
)

target_compile_features(nlm_test_all PRIVATE cxx_std_20)

# Compiler-specific settings
target_compile_options(nlm_test_all PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

# Platform-specific optimizations
target_link_libraries(nlm_test_all PRIVATE
    $<$<PLATFORM_ID:Linux>:-lm -ldl -pthread>
)

# Apply debug build optimizations for tests
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_test_all PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_libraries(nlm_test_all PRIVATE -fsanitize=address)
    endif()
endif()

# Add comprehensive test framework
function(nlm_add_test_with_fixtures test_name test_sources setup_code teardown_code)
    add_executable(${test_name} ${test_sources})
    
    # Link with test framework
    target_link_libraries(${test_name} PRIVATE
        nlm_core
        GTest::gtest
        GTest::gtest_main
    )
    
    # Apply common test settings
    target_compile_features(${test_name} PRIVATE cxx_std_20)
    target_compile_options(${test_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    )
    
    # Add test to CTest
    add_test(NAME ${test_name}
        COMMAND ${test_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    # Store test setup/teardown code for test framework
    set_target_properties(${test_name} PROPERTIES
        TEST_SETUP "${setup_code}"
        TEST_TEARDOWN "${teardown_code}"
    )
endfunction()

# Create specialized test targets
add_executable(nlm_unit_tests
    unit/core_types_test.cpp
    unit/config_test.cpp
    unit/random_test.cpp
)

target_link_libraries(nlm_unit_tests PRIVATE
    nlm_core
    GTest::gtest
    GTest::gtest_main
)

# Create integration test target
add_executable(nlm_integration_tests
    integration/brain_integration_test.cpp
    integration/memory_integration_test.cpp
    integration/plasticity_integration_test.cpp
)

target_link_libraries(nlm_integration_tests PRIVATE
    nlm_core
    nlm_agent
    nlm_world
    GTest::gtest
    GTest::gtest_main
)

# Add benchmark tests
add_executable(nlm_benchmark_tests
    benchmark/core_performance.cpp
    benchmark/plasticity_performance.cpp
)

target_link_libraries(nlm_benchmark_tests PRIVATE
    nlm_core
    nlm_agent
)

# Add performance profiling targets
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_benchmark_tests PRIVATE -g -O0)
    target_link_libraries(nlm_benchmark_tests PRIVATE valgrind/perf)
endif()

# Configure test environment
function(configure_test_environment)
    # Set up test directories
    set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_data)
    set(TEST_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_results)
    
    # Create directories
    file(MAKE_DIRECTORY ${TEST_OUTPUT_DIR})
    
    # Configure test environment variables
    set(TEST_SEED 12345 CACHE STRING "Random seed for tests")
    set(TEST_TIMEOUT 300 CACHE STRING "Test timeout in seconds")
    
    # Add environment setup
    add_test_environment_setup()
endfunction()

# Add custom test targets
add_custom_target(test_all
    COMMAND ctest --output-on-failure
    COMMENT "Run all NLM tests"
    VERBATIM
)

add_custom_target(test_unit
    COMMAND ctest -R unit
    COMMENT "Run unit tests only"
    VERBATIM
)

add_custom_target(test_integration
    COMMAND ctest -R integration
    COMMENT "Run integration tests only"
    VERBATIM
)

add_custom_target(test_benchmark
    COMMAND ctest -R benchmark
    COMMENT "Run benchmark tests only"
    VERBATIM
)

# Add test coverage targets
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Enable coverage
        target_compile_options(nlm_test_all PRIVATE --coverage)
        target_link_libraries(nlm_test_all PRIVATE --coverage)
        
        # Add coverage reporting target
        add_custom_target(coverage_report
            COMMAND gcovr --exclude test_*.cpp --exclude *gtest* --print-summary
            COMMAND gcovr --exclude test_*.cpp --exclude *gtest* --html --html-details
            COMMENT "Generate test coverage report"
            VERBATIM
        )
        
        # Add coverage upload target (for CI/CD)
        add_custom_target(coverage_upload
            COMMAND curl -s https://codecov.io/bash | bash -
            COMMENT "Upload coverage to Codecov"
            VERBATIM
        )
    endif()
endif()

# Add test dependency analysis
function(analyze_test_dependencies)
    find_program(DEPS_GRAPH plantuml)
    if (DEPS_GRAPH)
        add_custom_target(test_dependency_graph
            COMMAND plantuml -tsvg ${CMAKE_CURRENT_SOURCE_DIR}/test_dependencies.puml
            COMMENT "Generate test dependency graph"
            VERBATIM
        )
    endif()
endfunction()

# Generate test documentation
function(generate_test_documentation)
    find_package(Doxygen QUIET)
    if (DOXYGEN_FOUND)
        doxygen_add_docs(test_docs
            ${CMAKE_CURRENT_SOURCE_DIR}/docs/
            COMMENT "Generate test documentation"
        )
    endif()
endfunction()

# Run integration with main CMakeLists.txt targets
add_dependencies(nlm_test_all nlm_core)
add_dependencies(nlm_unit_tests nlm_core)
add_dependencies(nlm_integration_tests nlm_core nlm_agent nlm_world)

# Configure test environment
configure_test_environment()

# Generate documentation
generate_test_documentation()

# Analyze test dependencies
analyze_test_dependencies()

# Export test targets
export(TARGETS nlm_test_all nlm_unit_tests nlm_integration_tests nlm_benchmark_tests
       FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMTestTargets.cmake")
export(PACKAGE NLMTest)
