cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create comprehensive test executable with all tests
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
    test_brain_enhanced.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Add comprehensive test target that runs all test suites
add_custom_target(nlm_all_tests DEPENDS nlm_test)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add test target for enhanced tests
add_custom_target(nlm_enhanced_tests DEPENDS nlm_test COMMAND nlm_test -e
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Performance test target
add_executable(nlm_perf_test
    test_performance.cpp
)
target_link_libraries(nlm_perf_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_perf_test PRIVATE cxx_std_20)
add_test(NAME nlm_perf_test COMMAND nlm_perf_test)

# Memory management test target
add_executable(nlm_memory_test
    test_memory.cpp
)
target_link_libraries(nlm_memory_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_memory_test PRIVATE cxx_std_20)
add_test(NAME nlm_memory_test COMMAND nlm_memory_test)

# Performance benchmarking target
add_executable(nlm_benchmark
    ../src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PRIVATE nlm_core)
add_test(NAME nlm_benchmark COMMAND nlm_benchmark)

# Generate test coverage report
find_program(GCOV_COMMAND gcov)
if(GCOV_COMMAND)
    # Add coverage testing targets
    add_custom_target(test_coverage
        COMMAND ${GCOV_COMMAND} -b -c -d . -o coverage_report
        COMMAND lcov --remove coverage_report '*.o' '*/tests/*' --output-file coverage_report.filtered
        COMMAND genhtml coverage_report.filtered --output-directory coverage_html
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS nlm_test
    )
endif()
