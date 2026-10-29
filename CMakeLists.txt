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

# Build configuration options
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_DOCS "Build documentation" ON)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_DEMOS "Build demo executables" ON)
option(NLM_ENABLE_OPTIMIZATION "Enable compiler optimizations" ON)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library source files
set(NLM_CORE_SOURCES
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
    src/experiments/Phase3Experiment.cpp
    src/experiments/Phase4Experiment.cpp
    src/experiments/Phase5Experiment.cpp
    src/experiments/Phase6Experiment.cpp
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    src/experiments/Phase3Demo.cpp
    src/experiments/Phase4Demo.cpp
    src/experiments/Phase5Demo.cpp
    src/experiments/Phase6Demo.cpp
    src/experiments/Phase6IntegratedExperiment.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

# Core library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(NLM_ENABLE_OPTIMIZATION)
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -march=native>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /fp:fast>
    )
endif()

# Agent system (body, sensory percept, agent-brain interface)
set(NLM_AGENT_SOURCES
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})

target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)

target_include_directories(nlm_phase5 PUBLIC ${NLM_SOURCE_DIR})

target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase4)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6Experiment.cpp
    src/experiments/Phase6Demo.cpp
    src/experiments/Phase6IntegratedExperiment.cpp
)

target_include_directories(nlm_phase6 PUBLIC ${NLM_SOURCE_DIR})

target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase5)

# Main executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Demo executables
if(NLM_BUILD_DEMOS)
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    
    add_executable(nlm_phase5_demo
        src/experiments/Phase5Demo.cpp
    )
    
    target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)
    
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)
endif()

# Testing
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Python bindings
if(NLM_BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Documentation
if(NLM_BUILD_DOCS)
    # Add documentation build rules (requires doxygen)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
        set(DOXYGEN_PROJECT_NAME "NLM - Neural Learning Machine")
        set(DOXYGEN_PROJECT_NUMBER "Phase 6")
        doxygen_add_docs(docs ALL
            ${NLM_SOURCE_DIR}
            COMMENT "Generating documentation"
            OUTPUT_DIR ${DOXYGEN_OUTPUT_DIR}
        )
    endif()
endif()

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(NLM_BUILD_DEMOS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
        RUNTIME DESTINATION bin
    )
endif()

# Summary
message(STATUS "NLM Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Demos: ${NLM_BUILD_DEMOS}")
message(STATUS "  Optimization: ${NLM_ENABLE_OPTIMIZATION}")