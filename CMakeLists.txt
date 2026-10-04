# NLM - Neural Learning Machine
# Experimental artificial developmental brain project

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXTENSIONS OFF)

# Build type and options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings and debugging
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast)
endif()

# Optimization levels
option(NLM_ENABLE_OPTIMIZATION "Enable aggressive optimizations" ON)
option(NLM_ENABLE_ASSERTIONS "Enable runtime assertions" OFF)
option(NLM_ENABLE_PROFILING "Enable profiling support" OFF)
option(NLM_ENABLE_TESTING "Enable unit testing" ON)

if(NLM_ENABLE_OPTIMIZATION)
    add_compile_options(-O3 -march=native -flto)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(NLM_ENABLE_ASSERTIONS)
    add_compile_options(-DNDEBUG)
endif()

# Installation paths
set(NLM_INCLUDE_INSTALL_DIR "include" CACHE PATH "Installation directory for headers")
set(NLM_LIB_INSTALL_DIR "lib" CACHE PATH "Installation directory for libraries")
set(NLM_BIN_INSTALL_DIR "bin" CACHE PATH "Installation directory for executables")

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Platform-specific configurations
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux-specific optimizations
    add_compile_options(-pthread)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS-specific optimizations
    set(CMAKE_MACOSX_RPATH ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows-specific optimizations
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
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

# Core library configuration
target_compile_definitions(nlm_core PUBLIC
    $<$<BOOL:${NLM_ENABLE_ASSERTIONS}>:NLM_ENABLE_ASSERTIONS>
)

# Create configuration header
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Add configuration header generation
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config/Config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nlm_config.hpp"
    @ONLY
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

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

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

# Phase 6 demo executable (integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6)

# Python bindings support
option(NLM_ENABLE_PYTHON "Enable Python bindings" OFF)
if(NLM_ENABLE_PYTHON)
    # Find Python
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    
    # Add pybind11 target if not found
    if(NOT TARGET pybind11::module)
        find_package(pybind11 REQUIRED)
    endif()
    
    # Python module
    pybind11_add_module(pynlm
        src/python/PyNLM.cpp
        src/python/PyBrain.cpp
        src/python/PyNeuron.cpp
        src/python/PySynapse.cpp
    )
    target_link_libraries(pynlm PRIVATE nlm_core)
    set_target_properties(pynlm PROPERTIES
        OUTPUT_NAME "pynlm"
        PREFIX ""
        SUFFIX "$(PYTHON_MODULE_EXTENSION)"
    )
endif()

# CLI interface (optional)
option(NLM_ENABLE_CLI "Enable command-line interface" ON)
if(NLM_ENABLE_CLI)
    add_executable(nlm_cli
        src/cli/CLI.cpp
    )
    target_link_libraries(nlm_cli PUBLIC nlm_core nlm_agent nlm_world)
    set_target_properties(nlm_cli PROPERTIES
        OUTPUT_NAME "nlm-cli"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
