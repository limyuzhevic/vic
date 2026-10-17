cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

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

# Utilities library
add_library(nlm_utils STATIC
    src/utils/Utilities.cpp
)

target_include_directories(nlm_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/utils>
    $<INSTALL_INTERFACE:include/nlm>
)

# Error handling library
add_library(nlm_error_handling STATIC
    src/error_handling/EnhancedErrorHandling.cpp
)

target_include_directories(nlm_error_handling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/error_handling>
    $<INSTALL_INTERFACE:include/nlm>
)

# Update core libraries to use both utils and error handling
target_link_libraries(nlm_core PUBLIC nlm_utils nlm_error_handling)
target_link_libraries(nlm_agent PUBLIC nlm_utils nlm_error_handling)
target_link_libraries(nlm_world PUBLIC nlm_utils nlm_error_handling)

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
target_link_libraries(nlm PUBLIC nlm_core nlm_utils nlm_error_handling)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_utils nlm_error_handling)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_utils nlm_error_handling)

# Benchmark executable
add_executable(nlm_benchmarks
    src/benchmarks/Benchmarks.cpp
)
target_link_libraries(nlm_benchmarks PUBLIC nlm_core nlm_utils nlm_error_handling)

# Install benchmarks (optional)
install(TARGETS nlm_benchmarks
    RUNTIME DESTINATION bin
)

# Create CMake directory if it doesn't exist
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
endif()

# Generate and install package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION 0.1.0
    COMPATIBILITY AnyNewerVersion
)

# Create a simple config file since we don't have a full template
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "# NLM CMake Configuration File\n"
    "\n"
    "include(\"${CMAKE_CURRENT_LIST_DIR}/NLMTargets.cmake\")\n"
    "\n"
    "# Provide aliases for convenience\n"
    "add_library(nlm::core ALIAS nlm_core)\n"
    "add_library(nlm::agent ALIAS nlm_agent)\n"
    "add_library(nlm::world ALIAS nlm_world)\n"
    "\n"
    "if(TARGET nlm)\n"
    "    add_executable(nlm::nlm ALIAS nlm)\n"
    "endif()\n"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)
