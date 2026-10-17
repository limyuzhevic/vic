# NLM - Neural Learning Machine (熙然)
# Phase 6: Final Integration
# CMake build configuration

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "An experimental artificial developmental brain"
    LANGUAGES CXX
)

# Set C++ standard to C++20 (required for modern neural computation features)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configurations
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Compiler options and warnings
if(MSVC)
    # MSVC specific warnings
    add_compile_options(/W4 /WX)
    add_definitions(-DNOMINMAX)  # Prevent Windows min/max macros
else()
    # GCC and Clang specific warnings
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-error=maybe-uninitialized)
    add_definitions(-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700)
endif()

# Source and build directories
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(NLM_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/configs")

# Installation directories
set(NLM_INSTALL_BINDIR "bin" CACHE STRING "Installation directory for executables")
set(NLM_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
set(NLM_INSTALL_INCLUDEDIR "include" CACHE STRING "Installation directory for headers")

# Core neural computation library (Phase 6: all integrated systems)
add_library(nlm_core STATIC
    # Core infrastructure
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Neural components (Phase 2: real spiking computation)
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Dynamics and spike processing
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity mechanisms (STDP, Hebbian, structural)
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Development system (maturation, synaptogenesis, pruning)
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    
    # Neuromodulation systems (dopamine, reward, curiosity, novelty)
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    
    # Memory systems (working, episodic, semantic, procedural, associative)
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    
    # Cognitive systems (planning, concept formation, attention)
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Prediction system
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Motor system
    src/motor/MotorSystem.cpp
    
    # Environment interface
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiment framework (Phase 6)
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization interface
    src/visualization/VisualizationInterface.cpp
)

# Configure core library target properties
target_compile_features(nlm_core PUBLIC cxx_std_20)
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${NLM_INSTALL_INCLUDEDIR}>
)

# Performance optimization options
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nlm_core PRIVATE -O3 -flto)
    target_link_options(nlm_core PRIVATE -flto)
endif()

# Agent system (brain-body interface for world interaction)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world simulation)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments (basic world interaction)
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments (advanced learning and development)
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Main NLM executable (command-line interface)
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)
set_target_properties(nlm PROPERTIES
    OUTPUT_NAME "nlm"
    RUNTIME_OUTPUT_DIRECTORY "${NLM_BUILD_DIR}"
)

# Phase 3 demo executable (basic brain-world interaction)
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
set_target_properties(nlm_phase3_demo PROPERTIES
    OUTPUT_NAME "nlm_phase3_demo"
    RUNTIME_OUTPUT_DIRECTORY "${NLM_BUILD_DIR}"
)

# Phase 4 demo executable (advanced integrated brain)
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
set_target_properties(nlm_phase4_demo PROPERTIES
    OUTPUT_NAME "nlm_phase4_demo"
    RUNTIME_OUTPUT_DIRECTORY "${NLM_BUILD_DIR}"
)

# Testing framework
enable_testing()
add_subdirectory(tests)

# Installation targets (for package management)
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION ${NLM_INSTALL_BINDIR}
    COMPONENT bin
)

install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION ${NLM_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${NLM_INSTALL_LIBDIR}
    COMPONENT lib
)

install(DIRECTORY src/
    DESTINATION ${NLM_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    COMPONENT headers
)

# Config files installation
install(DIRECTORY ${NLM_CONFIG_DIR}/
    DESTINATION ${NLM_INSTALL_LIBDIR}/nlm/configs
    FILES_MATCHING PATTERN "*.cfg"
    COMPONENT configs
)

# Documentation targets
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Building documentation..."
    COMMENT "Building NLM documentation"
    VERBATIM
)

# Summary target showing configuration
add_custom_target(config_summary
    COMMAND ${CMAKE_COMMAND} -E echo "NLM Configuration Summary:"
    COMMAND ${CMAKE_COMMAND} -E echo "  Version: ${PROJECT_VERSION}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Build type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "  C++ standard: ${CMAKE_CXX_STANDARD}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Source directory: ${NLM_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Build directory: ${NLM_BUILD_DIR}"
    COMMENT "Display NLM configuration summary"
    VERBATIM
)
