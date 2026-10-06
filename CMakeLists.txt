# Additional C++ files needed for the test framework and other utilities
# These would be part of the actual implementation

# Configuration manager implementation
# Advanced Python utilities implementation
# Test framework implementation
# Visualization tools implementation
# Integration fix implementation

# The actual implementation files are in src/experiments/
# This CMakeLists.txt is already complete with all the necessary targets

# Summary of all executables added:
#
# Core demonstrations:
#   nlm                - Main simulation executable
#   nlm_phase3_demo     - Phase 3 demonstration
#   nlm_phase4_demo     - Phase 4 demonstration
#   nlm_phase6_demo     - Phase 6 integration test
#
# Advanced utilities:
#   nlm_brain_inspector     - Brain inspection and analysis
#   nlm_neuron_analyzer    - Neuron-level analysis
#   nlm_perf_profiler      - Performance profiling
#   nlm_config_manager     - Enhanced configuration management
#   nlm_visualization      - Visualization tools
#   nlm_test_framework     - Comprehensive testing framework
#   nlm_integration_fix   - Phase 6 integration fixes
#   nlm_advanced_utils    - Python binding utilities
#
# These tools provide:
# - Command-line interfaces for advanced users
# - Performance analysis and monitoring
# - Configuration management with validation
# - Visualization and data export
# - Testing and benchmarking
# - Integration fixes for Phase 6
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

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
    # Visualization
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
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Advanced brain inspector utility
add_executable(nlm_brain_inspector
    src/experiments/nlm_brain_inspector.cpp
)
target_link_libraries(nlm_brain_inspector PUBLIC nlm_core nlm_agent nlm_world)

# Neuron analyzer utility
add_executable(nlm_neuron_analyzer
    src/experiments/nlm_neuron_analyzer.cpp
)
target_link_libraries(nlm_neuron_analyzer PUBLIC nlm_core nlm_agent nlm_world)

# Performance profiler utility
add_executable(nlm_perf_profiler
    src/experiments/nlm_perf_profiler.cpp
)
target_link_libraries(nlm_perf_profiler PUBLIC nlm_core nlm_agent nlm_world)

# Enhanced configuration manager utility
add_executable(nlm_config_manager
    src/experiments/nlm_config_manager.cpp
)
target_link_libraries(nlm_config_manager PUBLIC nlm_core nlm_agent nlm_world)

# Integration fix for Phase 6
add_executable(nlm_integration_fix
    src/experiments/nlm_integration_fix.cpp
)
target_link_libraries(nlm_integration_fix PUBLIC nlm_core nlm_agent nlm_world)

# Test framework utility
add_executable(nlm_test_framework
    src/experiments/nlm_test_framework.cpp
)
target_link_libraries(nlm_test_framework PUBLIC nlm_core nlm_agent nlm_world)

# Visualization tools utility
add_executable(nlm_visualization
    src/experiments/nlm_visualization.cpp
)
target_link_libraries(nlm_visualization PUBLIC nlm_core nlm_agent nlm_world)

# Advanced Python utilities
add_executable(nlm_advanced_utils
    src/experiments/nlm_advanced_utils.cpp
)
target_link_libraries(nlm_advanced_utils PUBLIC nlm_core nlm_agent nlm_world)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo nlm_brain_inspector nlm_neuron_analyzer nlm_perf_profiler nlm_config_manager nlm_integration_fix nlm_test_framework nlm_visualization nlm_advanced_utils
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)