cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Include nlohmann/json library (header-only JSON library)
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
set(NLM_PROFILING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/profiling")

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
    src/experiments/BrainClone.cpp
    src/experiments/EnvironmentClone.cpp
    src/experiments/ExperimentResult.cpp
    src/experiments/BatchExperimentManager.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
    src/visualization/VisualizationFramework.cpp
    src/visualization/ConsoleVisualizer.cpp
    src/visualization/GUIVisualizer.cpp
    src/visualization/WebVisualizer.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Profiling framework
add_library(nlm_profiling STATIC
    src/profiling/ProfilingFramework.cpp
    src/profiling/CPUProfiler.cpp
    src/profiling/MemoryProfiler.cpp
    src/profiling/ProfileCPU.cpp
)
target_include_directories(nlm_profiling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/profiling>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_profiling PUBLIC nlm_core nlohmann_json::nlohmann_json)

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
target_link_libraries(nlm PUBLIC nlm_core nlohmann_json::nlohmann_json nlm_profiling)

# CLI executable
add_executable(nlm-cli
    src/main_cli.cpp
)
target_link_libraries(nlm-cli PUBLIC nlm_core nlohmann_json::nlohmann_json nlm_profiling)

# CLI tool executables
add_executable(nlm-cli-config-info
    src/tools/config_info_tool.cpp
)
target_link_libraries(nlm-cli-config-info PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-exp-info
    src/tools/experiment_info_tool.cpp
)
target_link_libraries(nlm-cli-exp-info PUBLIC nlm_core nlohmann_json::nlohmann_json)

# Visualization tools
add_executable(nlm-cli-viz-neuron
    src/main_cli.cpp
    tools/viz/viz-neuron.cpp
)
target_link_libraries(nlm-cli-viz-neuron PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-network
    src/main_cli.cpp
    tools/viz/viz-network.cpp
)
target_link_libraries(nlm-cli-viz-network PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-dynamics
    src/main_cli.cpp
    tools/viz/viz-dynamics.cpp
)
target_link_libraries(nlm-cli-viz-dynamics PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-synapses
    src/main_cli.cpp
    tools/viz/viz-synapses.cpp
)
target_link_libraries(nlm-cli-viz-synapses PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-network-dynamics
    src/main_cli.cpp
    tools/viz/viz-network-dynamics.cpp
)
target_link_libraries(nlm-cli-viz-network-dynamics PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-activity-patterns
    src/main_cli.cpp
    tools/viz/viz-activity-patterns.cpp
)
target_link_libraries(nlm-cli-viz-activity-patterns PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-space-time
    src/main_cli.cpp
    tools/viz/viz-space-time.cpp
)
target_link_libraries(nlm-cli-viz-space-time PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-brain-structure
    src/main_cli.cpp
    tools/viz/viz-brain-structure.cpp
)
target_link_libraries(nlm-cli-viz-brain-structure PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-brain-connectivity
    src/main_cli.cpp
    tools/viz/viz-brain-connectivity.cpp
)
target_link_libraries(nlm-cli-viz-brain-connectivity PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-brain-activity
    src/main_cli.cpp
    tools/viz/viz-brain-activity.cpp
)
target_link_libraries(nlm-cli-viz-brain-activity PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-viz-experiment-flow
    src/main_cli.cpp
    tools/viz/viz-experiment-flow.cpp
)
target_link_libraries(nlm-cli-viz-experiment-flow PUBLIC nlm_core nlohmann_json::nlohmann_json)

# Monitoring tools
add_executable(nlm-cli-monitor-brain
    src/main_cli.cpp
    tools/monitor/monitor-brain.cpp
)
target_link_libraries(nlm-cli-monitor-brain PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-environment
    src/main_cli.cpp
    tools/monitor/monitor-environment.cpp
)
target_link_libraries(nlm-cli-monitor-environment PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-plasticity
    src/main_cli.cpp
    tools/monitor/monitor-plasticity.cpp
)
target_link_libraries(nlm-cli-monitor-plasticity PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-memory
    src/main_cli.cpp
    tools/monitor/monitor-memory.cpp
)
target_link_libraries(nlm-cli-monitor-memory PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-development
    src/main_cli.cpp
    tools/monitor/monitor-development.cpp
)
target_link_libraries(nlm-cli-monitor-development PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-neuromodulation
    src/main_cli.cpp
    tools/monitor/monitor-neuromodulation.cpp
)
target_link_libraries(nlm-cli-monitor-neuromodulation PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-plasticity
    src/main_cli.cpp
    tools/monitor/monitor-plasticity.cpp
)
target_link_libraries(nlm-cli-monitor-plasticity PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-memory
    src/main_cli.cpp
    tools/monitor/monitor-memory.cpp
)
target_link_libraries(nlm-cli-monitor-memory PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-development
    src/main_cli.cpp
    tools/monitor/monitor-development.cpp
)
target_link_libraries(nlm-cli-monitor-development PUBLIC nlm_core nlohmann_json::nlohmann_json)

add_executable(nlm-cli-monitor-neuromodulation
    src/main_cli.cpp
    tools/monitor/monitor-neuromodulation.cpp
)
target_link_libraries(nlm-cli-monitor-neuromodulation PUBLIC nlm_core nlohmann_json::nlohmann_json)

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

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
