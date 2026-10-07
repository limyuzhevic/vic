cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable
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

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Performance benchmark test
add_executable(nlm_benchmark_perf
    test_performance.cpp
)
target_link_libraries(nlm_benchmark_perf PRIVATE nlm_core)
add_test(NAME nlm_benchmark_perf COMMAND nlm_benchmark_perf)

# Python integration tests (if Python bindings are built)
if(BUILD_PYTHON)
    add_custom_target(python_tests
        COMMAND ${Python_EXECUTABLE} -m pytest python/tests/ -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running Python integration tests"
        VERBATIM
    )
endif()

# Coverage test target
if(ENABLE_COVERAGE)
    add_custom_target(coverage_test
        COMMAND ./nlm_test --coverage
        COMMAND ./benchmark --coverage
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS nlm_test benchmark
        COMMENT "Running tests with coverage"
        VERBATIM
    )
endif()

# Documentation test target
if(BUILD_DOCS)
    add_custom_target(docs_test
        COMMAND doxygen Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Building documentation"
        VERBATIM
    )
endif()