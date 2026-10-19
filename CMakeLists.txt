cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Advanced features option
option(ENABLE_ADVANCED_FEATURES "Enable advanced configuration and features" ON)
option(ENABLE_MEMORY_POOLS "Enable memory pool optimization" ON)
option(ENABLE_SIMD_VECTORIZATION "Enable SIMD vectorization" ON)
option(ENABLE_PARALLEL_PROCESSING "Enable parallel processing" OFF)
option(ENABLE_CHECKPOINTING "Enable checkpointing" ON)
option(ENABLE_REPLAY "Enable memory replay" OFF)
option(ENABLE_DEVELOPMENT "Enable developmental processes" ON)
option(ENABLE_PREDICTION "Enable prediction systems" ON)
option(ENABLE_MEMORY_INTEGRATION "Enable memory integration" OFF)
option(ENABLE_COGNITION_INTEGRATION "Enable cognition integration" OFF)
option(ENABLE_PERFORMANCE_MONITORING "Enable performance monitoring" ON)
option(ENABLE_ADVANCED_LOGGING "Enable advanced logging" OFF)
option(ENABLE_METALEARNING "Enable meta-learning" OFF)
option(ENABLE_TRANSFER_LEARNING "Enable transfer learning" OFF)
option(ENABLE_CURRICULUM_LEARNING "Enable curriculum learning" OFF)
option(ENABLE_PLUGIN_SYSTEM "Enable plugin system" ON)
option(ENABLE_EXPERIMENTAL_FEATURES "Enable experimental features" OFF)

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

# Phase 5 experiments (performance optimized)
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
    src/experiments/Phase5Demo.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

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

# Phase 5 demo executable (performance optimized)
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase5)

# Phase 6 demo executable (integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core)

# Advanced features library
if(ENABLE_ADVANCED_FEATURES)
    add_library(nlm_advanced STATIC
        src/agent/AdvancedFeatures.hpp
        src/agent/AgentBrainConfig.hpp
    )
    target_include_directories(nlm_advanced PUBLIC src)
    target_link_libraries(nlm_advanced PUBLIC nlm_core nlm_agent)
    
    # Link advanced features to executables
    target_link_libraries(nlm PUBLIC nlm_advanced)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_advanced)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_advanced)
    target_link_libraries(nlm_phase5_demo PUBLIC nlm_advanced)
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_advanced)
endif()

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Documentation generation
if(BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
    
    doxygen_add_docs(docs
        ${DOXYGEN_INPUT_DIR}/README.md
        ${DOXYGEN_INPUT_DIR}/src
        COMMENT "Generating API documentation"
    )
endif()
