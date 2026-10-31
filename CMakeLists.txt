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

# Configure dependency management
message(STATUS "=== NLM Build Configuration ===")
message(STATUS "Project: NLM Neural Learning Machine")
message(STATUS "Version: 0.1.0")
message(STATUS "C++ Standard: C++20")

# Try to find nlohmann/json package
set(NLOHMANN_JSON_FOUND FALSE)
find_package(nlohmann_json 3.11 QUIET)
if(nlohmann_json_FOUND)
    message(STATUS "Found nlohmann/json: ${nlohmann_json_VERSION}")
else()
    message(STATUS "nlohmann/json not found, using fallback implementation")
    set(NLOHMANN_JSON_FALLBACK 1)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/include)
endif()

# Try to find yaml-cpp package
set(YAML_CPP_FOUND FALSE)
find_package(yaml-cpp QUIET)
if(yaml_cpp_FOUND)
    message(STATUS "Found yaml-cpp: ${YAML_CPP_VERSION}")
else()
    message(STATUS "yaml-cpp not found, using fallback implementation")
    set(YAML_CPP_FALLBACK 1)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/include)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Create external include directory structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/include/nlohmann)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/include/yaml-cpp)

# Copy fallback implementations if they don't exist
if(NOT NLOHMANN_JSON_FOUND)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/include/nlohmann/json.hpp)
        message(STATUS "Installing nlohmann/json fallback implementation...")
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/external/include/nlohmann/json.hpp
             DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/external/include/nlohmann/)
    endif()
endif()

if(NOT YAML_CPP_FOUND)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/include/yaml-cpp/yaml.h)
        message(STATUS "Installing yaml-cpp fallback implementation...")
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/external/include/yaml-cpp/yaml.h
             DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/external/include/yaml-cpp/)
    endif()
endif()

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

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

message(STATUS "=== Build Configuration Complete ===")
message(STATUS "Dependencies: ${NLOHMANN_JSON_FOUND ? \"Real nlohmann/json\" : \"Fallback nlohmann/json\"}")
message(STATUS "Dependencies: ${YAML_CPP_FOUND ? \"Real yaml-cpp\" : \"Fallback yaml-cpp\"}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
