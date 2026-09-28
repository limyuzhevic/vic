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

# Optimization settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g)
endif()

# Performance optimizations
add_compile_options(
    -march=native          # Optimize for current CPU
    -ffast-math            # Relax IEEE compliance for performance
    -fomit-frame-pointer   # Reduce stack frame overhead
)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library with all source files organized by category
add_library(nlm_core STATIC
    # Core utilities
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    
    # Neural computation core
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    
    # Dynamics and spike propagation
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    
    # Plasticity systems
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    
    # Development and maturation
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    
    # Neuromodulation
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    
    # Memory systems
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    
    # Cognitive systems
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    ${NLM_SOURCE_DIR}/cognition/AttentionalSelection.cpp
    
    # Sensory systems
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    
    # Prediction and learning
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    
    # Motor and action systems
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/motor/Action.cpp
    
    # Environment and world simulation
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    
    # World system
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
    
    # Agent and interface systems
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBody.cpp
    
    # Experiments and research
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase5Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp
    
    # Performance and optimization systems
    ${NLM_SOURCE_DIR}/performance/CheckpointSystem.cpp
    ${NLM_SOURCE_DIR}/performance/MemoryPool.cpp
    ${NLM_SOURCE_DIR}/performance/SIMDVectorization.cpp
    ${NLM_SOURCE_DIR}/performance/ParallelProcessing.cpp
    ${NLM_SOURCE_DIR}/performance/EventQueue.cpp
    ${NLM_SOURCE_DIR}/performance/SparseConnectivity.cpp
    ${NLM_SOURCE_DIR}/performance/OptimizedBrain.cpp
    
    # Visualization and debugging
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBody.cpp
)
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR}/agent)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR}/world)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase experiments
add_library(nlm_phase3 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR}/experiments)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

add_library(nlm_phase4 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR}/experiments)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

add_library(nlm_phase5 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC ${NLM_SOURCE_DIR}/experiments)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

add_library(nlm_phase6 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC ${NLM_SOURCE_DIR}/experiments)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Main executable
add_executable(nlm
    ${NLM_SOURCE_DIR}/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 5 demo executable
add_executable(nlm_phase5_demo
    ${NLM_SOURCE_DIR}/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Phase 6 demo executable (full integration)
add_executable(nlm_phase6_demo
    ${NLM_SOURCE_DIR}/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

# Testing framework
enable_testing()

# Add performance benchmark targets
add_custom_target(performance_test
    COMMAND ${CMAKE_BINARY_DIR}/nlm_phase6_demo
    COMMENT "Run performance benchmark"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add code coverage target if available
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    find_program(GCOVR_PATH gcovr PATHS /usr/bin /usr/local/bin)
    if(GCOVR_PATH)
        add_custom_target(coverage
            COMMAND ${GCOVR_PATH} --exclude=".*/tests/.*" --exclude=".*/performance/.*" --html-detailed coverage.html
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated: coverage.html"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generate code coverage report"
        )
    endif()
endif()

# Install targets (for future packaging)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${NLM_SOURCE_DIR}/ DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
)

# Summary message
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Core library sources: ${nlm_core_SOURCES}")
message(STATUS "  Executables: nlm, nlm_phase3_demo, nlm_phase4_demo, nlm_phase5_demo, nlm_phase6_demo")
message(STATUS "  Available targets: nlm_core, nlm_agent, nlm_world, nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6")
message(STATUS "  Performance targets: performance_test, coverage")
