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

# Core library with improved structure
add_library(nlm_core STATIC
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Brain components
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity rules
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
    
    # Memory systems
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
    
    # Attention
    src/cognition/AttentionalSelection.cpp
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction system
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor system
    src/motor/MotorSystem.cpp
    
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiments framework
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
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 5 optimizations (if exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/experiments/Phase5Experiment.cpp")
    add_library(nlm_phase5 STATIC
        src/experiments/Phase5Experiment.cpp
    )
    target_include_directories(nlm_phase5 PUBLIC src)
    target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Main executable - Phase 6 integration
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

# Phase 6 demo executable (the main integration demonstration)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

if (TARGET nlm_phase5)
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_phase5)
endif()

# Testing
enable_testing()
add_subdirectory(tests)

# Installation targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Add custom targets for common operations
add_custom_target(config
    COMMAND echo "Configuration:"
    COMMAND echo "  Build type: ${CMAKE_BUILD_TYPE}"
    COMMAND echo "  C++ standard: ${CMAKE_CXX_STANDARD}"
    COMMAND echo "  Install prefix: ${CMAKE_INSTALL_PREFIX}"
    COMMENT "Display build configuration"
)

add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMENT "Run tests"
)

add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation target (requires doxygen)"
    COMMENT "Generate documentation"
)

# Summary of targets
message(STATUS "=== NLM Build Summary ===")
message(STATUS "Core library: nlm_core")
message(STATUS "Agent library: nlm_agent")
message(STATUS "World library: nlm_world")
message(STATUS "Main executable: nlm (Phase 6 integration)")
message(STATUS "Phase 3 demo: nlm_phase3_demo")
message(STATUS "Phase 4 demo: nlm_phase4_demo")
message(STATUS "Phase 6 demo: nlm_phase6_demo")
message(STATUS "=== Available Targets ===")
message(STATUS "  make nlm                # Build main integration")
message(STATUS "  make nlm_phase6_demo    # Build Phase 6 demo")
message(STATUS "  make check              # Run tests")
message(STATUS "  make config             # Show build configuration")
message(STATUS "  make docs               # Documentation target")