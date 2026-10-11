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
    test_batch.cpp
    test_performance.cpp
    test_enhanced_bindings.cpp
    test_error_handling.cpp
    test_configuration_validation.cpp
    # Visualization tests
    ../src/visualization/VisualizationTestRunner.cpp
    ../src/visualization/SpikeVisualizationSuite.cpp
    ../src/visualization/PerformanceVisualizer.cpp
    # CLI integration tests
    ../src/cli/VisualizationIntegration.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Additional visualization tests
add_executable(nlm_visualization_test
    ../src/visualization/VisualizationTestRunner.cpp
    ../src/visualization/SpikeVisualizationSuite.cpp
    ../src/visualization/PerformanceVisualizer.cpp
)

target_link_libraries(nlm_visualization_test PRIVATE nlm_core)
target_compile_features(nlm_visualization_test PRIVATE cxx_std_20)

enable_testing()
add_test(NAME nlm_visualization_test COMMAND nlm_visualization_test)

# CLI integration tests
add_executable(nlm_cli_integration_test
    ../src/cli/VisualizationIntegration.cpp
)

target_link_libraries(nlm_cli_integration_test PRIVATE nlm_core)
target_compile_features(nlm_cli_integration_test PRIVATE cxx_std_20)

enable_testing()
add_test(NAME nlm_cli_integration_test COMMAND nlm_cli_integration_test)

# Experiment tests
add_executable(nlm_experiment_test
    ../src/experiments/VisualizationExperiment.cpp
)

target_link_libraries(nlm_experiment_test PRIVATE nlm_core)
target_compile_features(nlm_experiment_test PRIVATE cxx_std_20)

enable_testing()
add_test(NAME nlm_experiment_test COMMAND nlm_experiment_test)

# Performance tests
add_executable(nlm_performance_test
    ../src/visualization/PerformanceVisualizer.cpp
)

target_link_libraries(nlm_performance_test PRIVATE nlm_core)
target_compile_features(nlm_performance_test PRIVATE cxx_std_20)

enable_testing()
add_test(NAME nlm_performance_test COMMAND nlm_performance_test)

# Additional visualization tests
add_executable(nlm_visualization_test
    src/visualization/VisualizationTestRunner.cpp
    src/visualization/SpikeVisualizationSuite.cpp
    src/visualization/PerformanceVisualizer.cpp
)

target_link_libraries(nlm_visualization_test PRIVATE nlm_core)
target_compile_features(nlm_visualization_test PRIVATE cxx_std_20)

enable_testing()
add_test(NAME nlm_visualization_test COMMAND nlm_visualization_test)
