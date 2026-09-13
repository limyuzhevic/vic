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

# Enable warnings and additional flags
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-DNOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wshadow -Wnon-virtual-dtor -Wold-style-cast)
    add_definitions(-DNOMINMAX -fPIC)
endif()

# Coverage options
if(NLM_COVERAGE OR CMAKE_BUILD_TYPE STREQUAL "Coverage")
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# Installation directories
include(GNUInstallDirs)
set(NLM_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/nlm CACHE PATH "Installation path for CMake files")

# Options for build features
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_ENABLE_VISUALIZATION "Enable visualization support" ON)
option(NLM_COVERAGE "Enable coverage reporting" OFF)
option(NLM_OPTIMIZE_FOR_PERFORMANCE "Optimize for performance" ON)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library
if(BUILD_SHARED_LIBS)
    add_library(nlm_core SHARED
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
        src/neuromodulation/Dopamine.cpp
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
else()
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
        src/neuromodulation/Dopamine.cpp
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
endif()

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

if(NLM_ENABLE_VISUALIZATION)
    target_compile_definitions(nlm_core PRIVATE NLM_VISUALIZATION_ENABLED)
endif()

# Agent system (body, sensory percept, agent-brain interface)
if(BUILD_SHARED_LIBS)
    add_library(nlm_agent SHARED
        src/agent/SensoryPercept.cpp
        src/agent/AgentBrain.cpp
    )
else()
    add_library(nlm_agent STATIC
        src/agent/SensoryPercept.cpp
        src/agent/AgentBrain.cpp
    )
endif()
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
if(BUILD_SHARED_LIBS)
    add_library(nlm_world SHARED
        src/world/SimpleWorld.cpp
    )
else()
    add_library(nlm_world STATIC
        src/world/SimpleWorld.cpp
    )
endif()
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Main executable (Phase 2 demo)
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world)
if(NLM_OPTIMIZE_FOR_PERFORMANCE)
    target_compile_options(nlm PRIVATE -O3 -march=native)
endif()

# Additional demo executables
add_executable(nlm_phase3_demo
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world)

add_executable(nlm_phase4_demo
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world)

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
include(CMakePackageConfigHelpers)

# Install libraries
install(TARGETS nlm_core nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN ".*" EXCLUDE
)

# Install CMake configuration files
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    INSTALL_DESTINATION ${NLM_INSTALL_CMAKE_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    DESTINATION ${NLM_INSTALL_CMAKE_DIR}
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    DESTINATION ${NLM_INSTALL_CMAKE_DIR}
)