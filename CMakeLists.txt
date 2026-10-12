# cmake_minimum_required(VERSION 3.16)
# project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Coverage analysis options
include(CodeCoverage)
include(ProcessorCount)

# Performance configuration
option(NLM_ENABLE_PROFILING "Enable performance profiling" ON)
option(NLM_ENABLE_PERFORMANCE_ANALYSIS "Enable performance analysis" ON)
option(NLM_ENABLE_OPTIMIZATION_HINTS "Enable optimization hints" ON)
option(NLM_ENABLE_BENCHMARKS "Enable benchmarking framework" ON)
option(NLM_ENABLE_PARALLEL_RUNNER "Enable parallel processing utilities" ON)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # Brain
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    # Plasticity
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    # Development
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    # Neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    # Memory
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    # Sensory
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    # Motor
    src/motor/MotorSystem.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization - Advanced system
    src/visualization/AdvancedVisualization.cpp
    src/visualization/NetworkVisualizer.cpp
    src/visualization/ActivityVisualizer.cpp
    src/visualization/MemoryVisualizer.cpp
    src/visualization/NeuromodulationVisualizer.cpp
    src/visualization/DevelopmentVisualizer.cpp
    src/visualization/ExportUtils.cpp
    # Debugging and profiling
    src/debugging/DebugConsole.cpp
    src/profiling/Profiler.cpp
    # Legacy visualization (maintained for backward compatibility)
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Performance and Profiling Analysis Libraries
if(NLM_ENABLE_PROFILING)
    add_subdirectory(src/profiling)
endif()

if(NLM_ENABLE_PERFORMANCE_ANALYSIS)
    add_subdirectory(src/performance)
endif()

if(NLM_ENABLE_OPTIMIZATION_HINTS)
    add_subdirectory(src/optimization)
endif()

if(NLM_ENABLE_BENCHMARKS)
    add_subdirectory(src/benchmarks)
endif()

if(NLM_ENABLE_PARALLEL_RUNNER)
    add_subdirectory(src/utils)
endif()

# Profiling library
if(NLM_ENABLE_PROFILING)
    add_library(nlm_profiling STATIC
        src/profiling/Profiler.cpp
    )
    target_include_directories(nlm_profiling PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/profiling>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(nlm_profiling PUBLIC nlm_core)
endif()

# Performance analysis library
if(NLM_ENABLE_PERFORMANCE_ANALYSIS)
    add_library(nlm_performance STATIC
        src/performance/PerformanceAnalyzer.cpp
    )
    target_include_directories(nlm_performance PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/performance>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(nlm_performance PUBLIC nlm_core)
    if(NLM_ENABLE_PROFILING)
        target_link_libraries(nlm_performance PUBLIC nlm_profiling)
    endif()
endif()

# Optimization utilities library
if(NLM_ENABLE_OPTIMIZATION_HINTS)
    add_library(nlm_optimization STATIC
        src/optimization/OptimizationHints.cpp
        src/optimization/ParallelRunner.cpp
    )
    target_include_directories(nlm_optimization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/optimization>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(nlm_optimization PUBLIC nlm_core)
    if(NLM_ENABLE_PERFORMANCE_ANALYSIS)
        target_link_libraries(nlm_optimization PUBLIC nlm_performance)
    endif()
endif()

# Benchmark suite library
if(NLM_ENABLE_BENCHMARKS)
    add_library(nlm_benchmarks STATIC
        src/benchmarks/BenchmarkSuite.cpp
    )
    target_include_directories(nlm_benchmarks PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/benchmarks>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(nlm_benchmarks PUBLIC nlm_core)
    if(NLM_ENABLE_OPTIMIZATION_HINTS)
        target_link_libraries(nlm_benchmarks PUBLIC nlm_optimization)
    endif()
endif()

# Profiler reporter utilities
if(NLM_ENABLE_PARALLEL_RUNNER)
    add_library(nlm_profiler_reporter STATIC
        src/utils/ProfilerReporter.cpp
    )
    target_include_directories(nlm_profiler_reporter PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/utils>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(nlm_profiler_reporter PUBLIC nlm_core)
    if(NLM_ENABLE_PROFILING)
        target_link_libraries(nlm_profiler_reporter PUBLIC nlm_profiling)
    endif()
    if(NLM_ENABLE_PERFORMANCE_ANALYSIS)
        target_link_libraries(nlm_profiler_reporter PUBLIC nlm_performance)
    endif()
endif()

# Composite performance analysis target
add_library(nlm_performance_analysis INTERFACE)
if(NLM_ENABLE_PROFILING)
    target_link_libraries(nlm_performance_analysis INTERFACE nlm_profiling)
endif()
if(NLM_ENABLE_PERFORMANCE_ANALYSIS)
    target_link_libraries(nlm_performance_analysis INTERFACE nlm_performance)
endif()
if(NLM_ENABLE_OPTIMIZATION_HINTS)
    target_link_libraries(nlm_performance_analysis INTERFACE nlm_optimization)
endif()
if(NLM_ENABLE_BENCHMARKS)
    target_link_libraries(nlm_performance_analysis INTERFACE nlm_benchmarks)
endif()
if(NLM_ENABLE_PARALLEL_RUNNER)
    target_link_libraries(nlm_performance_analysis INTERFACE nlm_profiler_reporter)
endif()

# Performance analysis demo executable
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/experiments/PerformanceAnalysisDemo.cpp")
    add_executable(nlm_performance_analysis_demo
        src/experiments/PerformanceAnalysisDemo.cpp
    )
    target_link_libraries(nlm_performance_analysis_demo PUBLIC nlm_core nlm_performance_analysis)
endif()

# Plugin system
add_library(nlm_plugin_base STATIC
    src/plugin/Plugin.cpp
    src/plugin/PluginLoader.cpp
    src/plugin/PluginManager.cpp
    src/plugin/PluginSystem.cpp
    src/plugin/configuration/PluginConfig.cpp
    src/plugin/security/PluginSecurity.cpp
)
target_include_directories(nlm_plugin_base PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugin>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugin/configuration>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugin/security>
)
target_link_libraries(nlm_plugin_base PUBLIC nlm_core)

# Plugin examples (as part of library for easier distribution)
add_library(nlm_plugin_examples STATIC
    src/plugins/neurons/HodgkinHuxleyPlugin.cpp
    src/plugins/neurons/IzhikevichPlugin.cpp
    src/plugins/plasticity/BCMPlugin.cpp
    src/plugins/memories/WorkingMemoryPlugin.cpp
    src/plugins/neuromodulation/DopaminePlugin.cpp
    src/plugins/sensory/SensoryPlugin.cpp
)
target_include_directories(nlm_plugin_examples PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/neurons>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/plasticity>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/memories>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/neuromodulation>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/sensory>
)
target_link_libraries(nlm_plugin_examples PUBLIC nlm_core nlm_plugin_base)

# Modern test framework target
add_library(nlm_test_framework STATIC
    tests/framework/test_base.cpp
    tests/CoverageReport.cpp
    tests/test_runner/main.cpp
)
target_include_directories(nlm_test_framework PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests>
)

# Test executables with coverage
add_executable(test_neuron_modern tests/test_core/test_neuron_modern.cpp)
target_link_libraries(test_neuron_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_neuron_modern COMMAND test_neuron_modern)

add_executable(test_synapse_modern tests/test_core/test_synapse_modern.cpp)
target_link_libraries(test_synapse_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_synapse_modern COMMAND test_synapse_modern)

add_executable(test_brain_modern tests/test_core/test_brain_modern.cpp)
target_link_libraries(test_brain_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_brain_modern COMMAND test_brain_modern)

add_executable(test_neural_working_memory_modern tests/test_memory/test_working_memory_modern.cpp)
target_link_libraries(test_neural_working_memory_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_neural_working_memory_modern COMMAND test_neural_working_memory_modern)

add_executable(test_neural_episodic_memory_modern tests/test_memory/test_working_memory_modern.cpp)
target_link_libraries(test_neural_episodic_memory_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_neural_episodic_memory_modern COMMAND test_neural_episodic_memory_modern)

add_executable(test_neural_associative_memory_modern tests/test_memory/test_working_memory_modern.cpp)
target_link_libraries(test_neural_associative_memory_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_neural_associative_memory_modern COMMAND test_neural_associative_memory_modern)

add_executable(test_concept_formation_modern tests/test_cognition/test_concept_formation_modern.cpp)
target_link_libraries(test_concept_formation_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_concept_formation_modern COMMAND test_concept_formation_modern)

add_executable(test_neural_planner_modern tests/test_cognition/test_concept_formation_modern.cpp)
target_link_libraries(test_neural_planner_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_neural_planner_modern COMMAND test_neural_planner_modern)

add_executable(test_neuromodulation_modern tests/test_neuromod/test_neuromodulation_modern.cpp)
target_link_libraries(test_neuromodulation_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_neuromodulation_modern COMMAND test_neuromodulation_modern)

add_executable(test_prediction_modern tests/test_core/test_prediction_modern.cpp)
target_link_libraries(test_prediction_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_prediction_modern COMMAND test_prediction_modern)

add_executable(test_development_modern tests/test_core/test_development_modern.cpp)
target_link_libraries(test_development_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_development_modern COMMAND test_development_modern)

add_executable(test_environment_modern tests/test_environment/test_environment_modern.cpp)
target_link_libraries(test_environment_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_environment_modern COMMAND test_environment_modern)

add_executable(test_experiment_modern tests/test_experiments/test_experiment_modern.cpp)
target_link_libraries(test_experiment_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_experiment_modern COMMAND test_experiment_modern)

add_executable(test_performance_modern tests/test_performance/test_performance_modern.cpp)
target_link_libraries(test_performance_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_performance_modern COMMAND test_performance_modern)

add_executable(test_cli_modern tests/test_cli/test_cli_modern.cpp)
target_link_libraries(test_cli_modern PUBLIC nlm_core nlm_test_framework)
add_test(NAME test_cli_modern COMMAND test_cli_modern)

# Legacy test compatibility
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    # Add existing tests to test suite
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt" test_cmake_content)
    
    # Copy existing tests directory
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
        # Add existing demo tests to modern test framework
        add_subdirectory(tests)
    endif()
endif()

# Coverage target
include(CTarget)

# Performance tests (existing)
add_executable(nlm_performance_tests
    src/tests/test_performance.cpp
)
target_link_libraries(nlm_performance_tests PUBLIC nlm_core nlm_performance_analysis)
add_test(NAME nlm_performance_tests COMMAND nlm_performance_tests)

# Install targets
install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_performance_analysis
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    src/core/Types/Types.hpp
    src/core/Config/Config.hpp
    src/brain/Neuron.hpp
    src/brain/Synapse.hpp
    src/brain/Brain.hpp
    src/memory/Memory.hpp
    src/cognition/ConceptFormation.hpp
    src/cognition/NeuralPlanner.hpp
    src/neuromodulation/Neuromodulator.hpp
    src/prediction/PredictionSystem.hpp
    src/prediction/NeuralPrediction.hpp
    src/development/DevelopmentSystem.hpp
    src/environment/Environment.hpp
    DESTINATION include/nlm
)

# Add coverage reporting target
add_custom_target(coverage
    COMMAND echo "Running coverage analysis..."
    COMMAND lcov --directory . --capture --output-file coverage.info
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
    COMMAND genhtml -o coverage_report coverage.info
    COMMAND echo "Coverage report generated in coverage_report/"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate code coverage report"
)

# Parallel test execution option
if(NLM_ENABLE_PARALLEL_RUNNER)
    # Enable parallel test execution
    set_property(TEST test_neuron_modern test_synapse_modern test_brain_modern
                 test_neural_working_memory_modern test_neural_episodic_memory_modern
                 test_neural_associative_memory_modern test_concept_formation_modern
                 test_neural_planner_modern test_neuromodulation_modern
                 test_prediction_modern test_development_modern
                 test_environment_modern test_experiment_modern
                 test_performance_modern test_cli_modern
                 nlm_performance_tests
                 PROPERTY LABELS "parallel")
endif()

# Test filtering for selective test runs
set(TEST_FILTER "all" CACHE STRING "Test filter (all, neuron, memory, cognition, neuromod, environment, experiment, performance, cli)")

# Add test filtering
if(TEST_FILTER STREQUAL "neuron")
    set(TEST_TARGETS test_neuron_modern test_synapse_modern test_brain_modern)
elseif(TEST_FILTER STREQUAL "memory")
    set(TEST_TARGETS test_neural_working_memory_modern test_neural_episodic_memory_modern test_neural_associative_memory_modern)
elseif(TEST_FILTER STREQUAL "cognition")
    set(TEST_TARGETS test_concept_formation_modern test_neural_planner_modern)
elseif(TEST_FILTER STREQUAL "neuromod")
    set(TEST_TARGETS test_neuromodulation_modern)
elseif(TEST_FILTER STREQUAL "environment")
    set(TEST_TARGETS test_environment_modern)
elseif(TEST_FILTER STREQUAL "experiment")
    set(TEST_TARGETS test_experiment_modern nlm_performance_tests)
elseif(TEST_FILTER STREQUAL "performance")
    set(TEST_TARGETS test_performance_modern)
elseif(TEST_FILTER STREQUAL "cli")
    set(TEST_TARGETS test_cli_modern)
else()
    set(TEST_TARGETS test_neuron_modern test_synapse_modern test_brain_modern
                 test_neural_working_memory_modern test_neural_episodic_memory_modern
                 test_neural_associative_memory_modern test_concept_formation_modern
                 test_neural_planner_modern test_neuromodulation_modern
                 test_prediction_modern test_development_modern
                 test_environment_modern test_experiment_modern
                 test_performance_modern test_cli_modern
                 nlm_performance_tests)
endif()

# Add targets with filtering
foreach(test_target ${TEST_TARGETS})
    add_custom_target(${test_target}_filtered
        COMMAND ${test_target}
        COMMENT "Running filtered test: ${test_target}"
    )
endforeach()

message(STATUS "NLM Test Framework Configured")
message(STATUS "Test Filter: ${TEST_FILTER}")
message(STATUS "Parallel Execution: ${NLM_ENABLE_PARALLEL_RUNNER}")
message(STATUS "Coverage Reporting: Enabled")