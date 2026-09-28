#define PROJECT_NAME NLM
#define PROJECT_VERSION "0.1.0"

# CMake minimum version
cmake_minimum_required(VERSION 3.16)

# Project declaration
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
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Collect all existing source files
file(GLOB_RECURSE NLM_CORE_SOURCES
    "${NLM_SOURCE_DIR}/src/core/Types/Types.cpp"
    "${NLM_SOURCE_DIR}/src/core/Config/Config.cpp"
    "${NLM_SOURCE_DIR}/src/core/Random/Random.cpp"
    "${NLM_SOURCE_DIR}/src/core/Logger/Logger.cpp"
    "${NLM_SOURCE_DIR}/src/core/SimulationClock/SimulationClock.cpp"
    "${NLM_SOURCE_DIR}/src/brain/Neuron.cpp"
    "${NLM_SOURCE_DIR}/src/brain/Synapse.cpp"
    "${NLM_SOURCE_DIR}/src/brain/NeuralPopulation.cpp"
    "${NLM_SOURCE_DIR}/src/brain/NeuralRegion.cpp"
    "${NLM_SOURCE_DIR}/src/brain/Brain.cpp"
    "${NLM_SOURCE_DIR}/src/dynamics/NeuralDynamics.cpp"
    "${NLM_SOURCE_DIR}/src/dynamics/SpikeSystem.cpp"
    "${NLM_SOURCE_DIR}/src/plasticity/PlasticityRule.cpp"
    "${NLM_SOURCE_DIR}/src/plasticity/Hebbian.cpp"
    "${NLM_SOURCE_DIR}/src/plasticity/STDP.cpp"
    "${NLM_SOURCE_DIR}/src/plasticity/StructuralPlasticity.cpp"
    "${NLM_SOURCE_DIR}/src/development/DevelopmentSystem.cpp"
    "${NLM_SOURCE_DIR}/src/development/Synaptogenesis.cpp"
    "${NLM_SOURCE_DIR}/src/development/Pruning.cpp"
    "${NLM_SOURCE_DIR}/src/development/Maturation.cpp"
    "${NLM_SOURCE_DIR}/src/neuromodulation/Neuromodulator.cpp"
    "${NLM_SOURCE_DIR}/src/neuromodulation/Reward.cpp"
    "${NLM_SOURCE_DIR}/src/neuromodulation/PredictionError.cpp"
    "${NLM_SOURCE_DIR}/src/neuromodulation/Novelty.cpp"
    "${NLM_SOURCE_DIR}/src/neuromodulation/Curiosity.cpp"
    "${NLM_SOURCE_DIR}/src/memory/Memory.cpp"
    "${NLM_SOURCE_DIR}/src/memory/NeuralWorkingMemory.cpp"
    "${NLM_SOURCE_DIR}/src/memory/NeuralEpisodicMemory.cpp"
    "${NLM_SOURCE_DIR}/src/cognition/ConceptFormation.cpp"
    "${NLM_SOURCE_DIR}/src/cognition/NeuralPlanner.cpp"
    "${NLM_SOURCE_DIR}/src/sensory/SensoryInput.cpp"
    "${NLM_SOURCE_DIR}/src/sensory/Vision.cpp"
    "${NLM_SOURCE_DIR}/src/sensory/Audio.cpp"
    "${NLM_SOURCE_DIR}/src/sensory/InternalSignals.cpp"
    "${NLM_SOURCE_DIR}/src/prediction/PredictionSystem.cpp"
    "${NLM_SOURCE_DIR}/src/prediction/NeuralPrediction.cpp"
    "${NLM_SOURCE_DIR}/src/motor/MotorSystem.cpp"
    "${NLM_SOURCE_DIR}/src/motor/Action.cpp"
    "${NLM_SOURCE_DIR}/src/environment/Environment.cpp"
    "${NLM_SOURCE_DIR}/src/environment/Observation.cpp"
    "${NLM_SOURCE_DIR}/src/experiments/Experiment.cpp"
    "${NLM_SOURCE_DIR}/src/experiments/ExperimentRunner.cpp"
    "${NLM_SOURCE_DIR}/src/experiments/Metrics.cpp"
    "${NLM_SOURCE_DIR}/src/visualization/VisualizationInterface.cpp"
)

# Core library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system
file(GLOB_RECURSE NLM_AGENT_SOURCES
    "${NLM_SOURCE_DIR}/src/agent/SensoryPercept.cpp"
    "${NLM_SOURCE_DIR}/src/agent/AgentBrain.cpp"
)

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR}/src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
file(GLOB_RECURSE NLM_WORLD_SOURCES
    "${NLM_SOURCE_DIR}/src/world/SimpleWorld.cpp"
    "${NLM_SOURCE_DIR}/src/world/Observation.cpp"
)

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR}/src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# CLI library
file(GLOB_RECURSE NLM_CLI_SOURCES
    "${NLM_SOURCE_DIR}/src/cli/AdvancedCLI.cpp"
    "${NLM_SOURCE_DIR}/src/cli/ConfigurationManager.cpp"
    "${NLM_SOURCE_DIR}/src/cli/SimulationController.cpp"
    "${NLM_SOURCE_DIR}/src/cli/CLIMain.cpp"
)

add_library(nlm_cli STATIC ${NLM_CLI_SOURCES})
target_include_directories(nlm_cli PUBLIC ${NLM_SOURCE_DIR}/src)
target_link_libraries(nlm_cli PUBLIC nlm_core nlm_agent nlm_world)

# Phase 3 experiments
file(GLOB_RECURSE NLM_PHASE3_SOURCES
    "${NLM_SOURCE_DIR}/src/experiments/Phase3Experiment.cpp"
)

add_library(nlm_phase3 STATIC ${NLM_PHASE3_SOURCES})
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR}/src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
file(GLOB_RECURSE NLM_PHASE4_SOURCES
    "${NLM_SOURCE_DIR}/src/experiments/Phase4Experiment.cpp"
)

add_library(nlm_phase4 STATIC ${NLM_PHASE4_SOURCES})
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR}/src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm ${NLM_SOURCE_DIR}/src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core nlm_cli)

# CLI executable
add_executable(nlm_cli_main ${NLM_SOURCE_DIR}/src/cli/CLIMain.cpp)
target_link_libraries(nlm_cli_main PUBLIC nlm_core nlm_cli)

# Phase 3 demo executable
add_executable(nlm_phase3_demo ${NLM_SOURCE_DIR}/src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo ${NLM_SOURCE_DIR}/src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_cli nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Print configuration summary
message(STATUS "NLM Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source directory: ${NLM_SOURCE_DIR}")
message(STATUS "  Core source files: ${NLM_CORE_SOURCES}")
message(STATUS "  Total core files: ${NLM_CORE_SOURCES_COUNT}")