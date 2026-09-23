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

target_link_libraries(nlm_test PRIVATE nlm_core nlm_performance)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add new test executables
add_executable(nlm_test_logger
    test_logger.cpp
)
target_link_libraries(nlm_test_logger PRIVATE nlm_core)

add_executable(nlm_test_config
    test_config.cpp
)
target_link_libraries(nlm_test_config PRIVATE nlm_core)

add_executable(nlm_test_batch_processor
    test_batch_processor.cpp
)
target_link_libraries(nlm_test_batch_processor PRIVATE nlm_core nlm_performance)

add_executable(nlm_test_performance_profiler
    test_performance_profiler.cpp
)
target_link_libraries(nlm_test_performance_profiler PRIVATE nlm_core nlm_performance)

add_executable(nlm_test_python_bindings
    test_python_bindings.cpp
)
target_link_libraries(nlm_test_python_bindings PRIVATE nlm_core nlm_performance)

add_executable(nlm_test_system_integration
    test_system_integration.cpp
)
target_link_libraries(nlm_test_system_integration PRIVATE nlm_core nlm_performance)

add_executable(nlm_test_performance_benchmarks
    test_performance_benchmarks.cpp
)
target_link_libraries(nlm_test_performance_benchmarks PRIVATE nlm_core nlm_performance)

add_executable(nlm_test_memory_management
    test_memory_management.cpp
)
target_link_libraries(nlm_test_memory_management PRIVATE nlm_core nlm_performance)

add_executable(nlm_test_error_handling
    test_error_handling.cpp
)
target_link_libraries(nlm_test_error_handling PRIVATE nlm_core nlm_performance)

# Add tests to CTest
enable_testing()
add_test(NAME nlm_test_logger COMMAND nlm_test_logger)
add_test(NAME nlm_test_config COMMAND nlm_test_config)
add_test(NAME nlm_test_batch_processor COMMAND nlm_test_batch_processor)
add_test(NAME nlm_test_performance_profiler COMMAND nlm_test_performance_profiler)
add_test(NAME nlm_test_python_bindings COMMAND nlm_test_python_bindings)
add_test(NAME nlm_test_system_integration COMMAND nlm_test_system_integration)
add_test(NAME nlm_test_performance_benchmarks COMMAND nlm_test_performance_benchmarks)
add_test(NAME nlm_test_memory_management COMMAND nlm_test_memory_management)
add_test(NAME nlm_test_error_handling COMMAND nlm_test_error_handling)
