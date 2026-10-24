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
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Build options
option(NLM_ENABLE_PERF_OPTIMIZATIONS "Enable performance optimizations (SIMD, parallelization)" ON)
option(NLM_ENABLE_VISUALIZATION "Enable visualization support" OFF)
option(NLM_ENABLE_CHECKPOINTING "Enable checkpoint save/load functionality" ON)
option(NLM_ENABLE_REPLAY "Enable episodic memory replay" ON)
option(NLM_ENABLE_DETAILED_LOGGING "Enable detailed logging for debugging" OFF)
option(NLM_BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_TESTS "Build test suite" ON)
option(NLM_BUILD_EXPERIMENTS "Build experiment executables" ON)

# Performance optimizations
if(NLM_ENABLE_PERF_OPTIMIZATIONS)
    add_definitions(-DNLM_PERF_OPTIMIZED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -O3")
    
    # Performance libraries
    find_package(Threads REQUIRED)
    
    # Link performance libraries
    target_link_libraries(nlm_core PUBLIC Threads::Threads)
endif()

# Python bindings
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(pybind11 REQUIRED)
    
    pybind11_add_module(pynlm
        python/bindings.cpp
    )
    
    target_include_directories(pynlm PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${pybind11_INCLUDE_DIR}
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core)
    
    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
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

# Apply performance optimizations to core library
if(NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_compile_options(nlm_core PRIVATE -march=native -O3 -flto)
    set_target_properties(nlm_core PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

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

# Phase 6 demo executable (integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world)

# Testing
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Experiments
if(NLM_BUILD_EXPERIMENTS)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Add build summary
include(CMakePrintHelpers)
message(STATUS "NLM Build Configuration:")
message(STATUS "  C++ Standard: C++20")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Performance optimizations: ${NLM_ENABLE_PERF_OPTIMIZATIONS}")
message(STATUS "  Visualization support: ${NLM_ENABLE_VISUALIZATION}")
message(STATUS "  Checkpointing: ${NLM_ENABLE_CHECKPOINTING}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Test suite: ${NLM_BUILD_TESTS}")
message(STATUS "  Experiments: ${NLM_BUILD_EXPERIMENTS}")
