cmake_minimum_required(VERSION 3.16)

# Test configuration
set(TEST_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Enable testing
enable_testing()

# Compiler and test settings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Core Unit Test Targets
add_executable(unit_core_test
    core/core_unit_tests.cpp
)
target_link_libraries(unit_core_test PRIVATE nlm_core)
target_compile_features(unit_core_test PRIVATE cxx_std_20)

# Brain Unit Test Targets
add_executable(unit_brain_test
    brain/brain_unit_tests.cpp
    brain/neuron_unit_tests.cpp
    brain/synapse_unit_tests.cpp
    brain/neural_population_unit_tests.cpp
    brain/neural_region_unit_tests.cpp
)
target_link_libraries(unit_brain_test PRIVATE nlm_core nlm_agent)
target_compile_features(unit_brain_test PRIVATE cxx_std_20)

# Plasticity Unit Test Targets
add_executable(unit_plasticity_test
    plasticity/stdp_unit_tests.cpp
    plasticity/hebbian_unit_tests.cpp
    plasticity/structural_plasticity_unit_tests.cpp
)
target_link_libraries(unit_plasticity_test PRIVATE nlm_core)
target_compile_features(unit_plasticity_test PRIVATE cxx_std_20)

# Neuromodulation Unit Test Targets
add_executable(unit_neuromodulation_test
    neuromodulation/reward_modulation_unit_tests.cpp
    neuromodulation/curiosity_unit_tests.cpp
    neuromodulation/novelty_detection_unit_tests.cpp
    neuromodulation/prediction_error_unit_tests.cpp
)
target_link_libraries(unit_neuromodulation_test PRIVATE nlm_core)
target_compile_features(unit_neuromodulation_test PRIVATE cxx_std_20)

# Memory Unit Test Targets
add_executable(unit_memory_test
    memory/working_memory_unit_tests.cpp
    memory/episodic_memory_unit_tests.cpp
    memory/semantic_memory_unit_tests.cpp
    memory/procedural_memory_unit_tests.cpp
    memory/associative_memory_unit_tests.cpp
)
target_link_libraries(unit_memory_test PRIVATE nlm_core)
target_compile_features(unit_memory_test PRIVATE cxx_std_20)

# Cognition Unit Test Targets
add_executable(unit_cognition_test
    cognition/concept_formation_unit_tests.cpp
    cognition/neural_planner_unit_tests.cpp
)
target_link_libraries(unit_cognition_test PRIVATE nlm_core)
target_compile_features(unit_cognition_test PRIVATE cxx_std_20)

# Sensory Unit Test Targets
add_executable(unit_sensory_test
    sensory/vision_unit_tests.cpp
    sensory/audio_unit_tests.cpp
    sensory/internal_signals_unit_tests.cpp
)
target_link_libraries(unit_sensory_test PRIVATE nlm_core)
target_compile_features(unit_sensory_test PRIVATE cxx_std_20)

# Motor Unit Test Targets
add_executable(unit_motor_test
    motor/motor_system_unit_tests.cpp
    motor/action_unit_tests.cpp
)
target_link_libraries(unit_motor_test PRIVATE nlm_core)
target_compile_features(unit_motor_test PRIVATE cxx_std_20)

# Environment Unit Test Targets
add_executable(unit_environment_test
    environment/environment_unit_tests.cpp
    environment/observation_unit_tests.cpp
    environment/action_unit_tests.cpp
)
target_link_libraries(unit_environment_test PRIVATE nlm_core)
target_compile_features(unit_environment_test PRIVATE cxx_std_20)

# World Unit Test Targets
add_executable(unit_world_test
    world/simple_world_unit_tests.cpp
    world/world_object_unit_tests.cpp
    world/physics_simulation_unit_tests.cpp
)
target_link_libraries(unit_world_test PRIVATE nlm_core)
target_compile_features(unit_world_test PRIVATE cxx_std_20)

# Agent Unit Test Targets
add_executable(unit_agent_test
    agent/agent_brain_unit_tests.cpp
    agent/agent_body_unit_tests.cpp
    agent/sensory_percept_unit_tests.cpp
)
target_link_libraries(unit_agent_test PRIVATE nlm_core)
target_compile_features(unit_agent_test PRIVATE cxx_std_20)

# Development Unit Test Targets
add_executable(unit_development_test
    development/synaptogenesis_unit_tests.cpp
    development/pruning_unit_tests.cpp
    development/maturation_unit_tests.cpp
)
target_link_libraries(unit_development_test PRIVATE nlm_core)
target_compile_features(unit_development_test PRIVATE cxx_std_20)

# Prediction Unit Test Targets
add_executable(unit_prediction_test
    prediction/neural_prediction_unit_tests.cpp
    prediction/prediction_system_unit_tests.cpp
)
target_link_libraries(unit_prediction_test PRIVATE nlm_core)
target_compile_features(unit_prediction_test PRIVATE cxx_std_20)

# Phase-Specific Test Targets
add_executable(phase_2_test
    phase_specific/phase2_core_computation_unit_tests.cpp
)
target_link_libraries(phase_2_test PRIVATE nlm_core)
target_compile_features(phase_2_test PRIVATE cxx_std_20)

add_executable(phase_3_test
    phase_specific/phase3_experiments.cpp
)
target_link_libraries(phase_3_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3)
target_compile_features(phase_3_test PRIVATE cxx_std_20)

add_executable(phase_4_test
    phase_specific/phase4_experiments.cpp
)
target_link_libraries(phase_4_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
target_compile_features(phase_4_test PRIVATE cxx_std_20)

add_executable(phase_5_test
    phase_specific/phase5_experiments.cpp
)
target_link_libraries(phase_5_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase5)
target_compile_features(phase_5_test PRIVATE cxx_std_20)

add_executable(phase_6_test
    phase_specific/phase6_integration_tests.cpp
)
target_link_libraries(phase_6_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)
target_compile_features(phase_6_test PRIVATE cxx_std_20)

# Integration Test Targets
add_executable(integration_tests
    integration/sensorimotor_integration_tests.cpp
    integration/memory_coordination_tests.cpp
    integration/plasticity_neuromodulation_integration_tests.cpp
)
target_link_libraries(integration_tests PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(integration_tests PRIVATE cxx_std_20)

# Performance Test Targets
add_executable(performance_tests
    performance/speed_benchmarks.cpp
    performance/memory_usage_benchmarks.cpp
    performance/scalability_tests.cpp
)
target_link_libraries(performance_tests PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(performance_tests PRIVATE cxx_std_20)

# Configuration Test Targets
add_executable(config_tests
    config/configuration_loading_tests.cpp
    config/validation_tests.cpp
    config/cli_integration_tests.cpp
)
target_link_libraries(config_tests PRIVATE nlm_core)
target_compile_features(config_tests PRIVATE cxx_std_20)

# Experiment Test Targets
add_executable(experiment_tests
    experiment/experiment_runner_tests.cpp
    experiment/metrics_tests.cpp
    experiment/data_collection_tests.cpp
)
target_link_libraries(experiment_tests PRIVATE nlm_core)
target_compile_features(experiment_tests PRIVATE cxx_std_20)

# Python Binding Test Targets
add_executable(python_bindings_tests
    python/bindings_tests.cpp
)
target_link_libraries(python_bindings_tests PRIVATE nlm_core)
target_compile_features(python_bindings_tests PRIVATE cxx_std_20)

# Visualization Test Targets
add_executable(visualization_tests
    visualization/visualization_interface_tests.cpp
    visualization/data_output_tests.cpp
)
target_link_libraries(visualization_tests PRIVATE nlm_core)
target_compile_features(visualization_tests PRIVATE cxx_std_20)

# Legacy test targets (for backward compatibility)
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

# Create validation test executable
add_executable(validation_test
    ../src/core/Config/ValidationTest.cpp
)
target_link_libraries(validation_test PRIVATE nlm_core)
target_compile_features(validation_test PRIVATE cxx_std_20)

# Create framework test utility executable
add_executable(framework_tests
    framework/framework_test_runner.cpp
)
target_link_libraries(framework_tests PRIVATE nlm_core)
target_compile_features(framework_tests PRIVATE cxx_std_20)

# Test categorization helper target
add_custom_target(run_unit_tests
    COMMAND echo "=== Unit Tests Category ==="
    COMMAND ./unit_core_test
    COMMAND ./unit_brain_test
    COMMAND ./unit_plasticity_test
    COMMAND ./unit_neuromodulation_test
    COMMAND ./unit_memory_test
    COMMAND ./unit_cognition_test
    COMMAND ./unit_sensory_test
    COMMAND ./unit_motor_test
    COMMAND ./unit_environment_test
    COMMAND ./unit_world_test
    COMMAND ./unit_agent_test
    COMMAND ./unit_development_test
    COMMAND ./unit_prediction_test
    COMMAND echo "  Unit tests completed"
    COMMENT "Running all unit tests"
)

add_custom_target(run_integration_tests
    COMMAND echo "=== Integration Tests Category ==="
    COMMAND ./integration_tests
    COMMAND ./phase_2_test
    COMMAND ./phase_3_test
    COMMAND ./phase_4_test
    COMMAND ./phase_5_test
    COMMAND ./phase_6_test
    COMMAND ./framework_tests
    COMMAND echo "  Integration tests completed"
    COMMENT "Running integration tests"
)

add_custom_target(run_comprehensive_tests
    COMMAND echo "=== Running All Tests ==="
    COMMAND ./nlm_test
    COMMAND ./validation_test
    COMMAND ./performance_tests
    COMMAND ./config_tests
    COMMAND ./experiment_tests
    COMMAND ./python_bindings_tests
    COMMAND ./visualization_tests
    COMMAND ./framework_tests
    COMMENT "Running all tests"
)

add_custom_target(run_python_bindings_tests
    COMMAND python3 ${TEST_ROOT_DIR}/test_enhanced_bindings.py
    COMMENT "Running Python binding tests"
)

# Add all tests to test suite
add_test(NAME unit_core_test COMMAND unit_core_test)
add_test(NAME unit_brain_test COMMAND unit_brain_test)
add_test(NAME unit_plasticity_test COMMAND unit_plasticity_test)
add_test(NAME unit_neuromodulation_test COMMAND unit_neuromodulation_test)
add_test(NAME unit_memory_test COMMAND unit_memory_test)
add_test(NAME unit_cognition_test COMMAND unit_cognition_test)
add_test(NAME unit_sensory_test COMMAND unit_sensory_test)
add_test(NAME unit_motor_test COMMAND unit_motor_test)
add_test(NAME unit_environment_test COMMAND unit_environment_test)
add_test(NAME unit_world_test COMMAND unit_world_test)
add_test(NAME unit_agent_test COMMAND unit_agent_test)
add_test(NAME unit_development_test COMMAND unit_development_test)
add_test(NAME unit_prediction_test COMMAND unit_prediction_test)
add_test(NAME phase_2_test COMMAND phase_2_test)
add_test(NAME phase_3_test COMMAND phase_3_test)
add_test(NAME phase_4_test COMMAND phase_4_test)
add_test(NAME phase_5_test COMMAND phase_5_test)
add_test(NAME phase_6_test COMMAND phase_6_test)
add_test(NAME integration_tests COMMAND integration_tests)
add_test(NAME performance_tests COMMAND performance_tests)
add_test(NAME config_tests COMMAND config_tests)
add_test(NAME experiment_tests COMMAND experiment_tests)
add_test(NAME python_bindings_tests COMMAND python_bindings_tests)
add_test(NAME visualization_tests COMMAND visualization_tests)
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME validation_test COMMAND validation_test)
add_test(NAME framework_tests COMMAND framework_tests)

# Create test data directory
add_custom_target(create_test_data
    COMMAND mkdir -p ${TEST_ROOT_DIR}/data
    COMMAND mkdir -p ${TEST_ROOT_DIR}/results
    COMMAND echo "Test data directories created"
    COMMENT "Creating test data directories"
)

# Test reporting
add_custom_target(report_tests
    COMMAND echo "=== Test Results Report ==="
    COMMAND echo "Unit tests executable: ./unit_core_test ./unit_brain_test ./unit_plasticity_test ./unit_neuromodulation_test ./unit_memory_test ./unit_cognition_test ./unit_sensory_test ./unit_motor_test ./unit_environment_test ./unit_world_test ./unit_agent_test ./unit_development_test ./unit_prediction_test"
    COMMAND echo "Integration tests executable: ./integration_tests ./phase_2_test ./phase_3_test ./phase_4_test ./phase_5_test ./phase_6_test ./framework_tests"
    COMMAND echo "Legacy tests executable: ./nlm_test ./validation_test"
    COMMAND echo "Python binding tests: python3 test_enhanced_bindings.py"
    COMMAND echo "Performance tests executable: ./performance_tests"
    COMMAND echo "Configuration tests executable: ./config_tests"
    COMMAND echo "Experiment tests executable: ./experiment_tests"
    COMMAND echo "Python binding tests executable: ./python_bindings_tests"
    COMMAND echo "Visualization tests executable: ./visualization_tests"
    COMMENT "Reporting test configuration"
)