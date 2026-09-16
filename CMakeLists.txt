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

# Core library - modular structure for better dependency management
set(NLM_CORE_SOURCES
    # Core utilities
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    
    # Brain components (Phase 6 integration)
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    
    # Neural dynamics
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    
    # Plasticity rules
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    
    # Development system
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    
    # Neuromodulation systems (Phase 6)
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    
    # Memory systems (Phase 6 integration)
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    
    # Cognition systems (Phase 6 integration)
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    
    # Prediction systems (Phase 6 integration)
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    
    # Motor system
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    
    # Environment interface
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    
    # Experiments (Phase 6)
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase5Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp
    
    # Visualization
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
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

# Performance libraries (Phase 5 optimizations)
if(NLM_ENABLE_PERFORMANCE)
    add_library(nlm_performance STATIC
        src/performance/MemoryPool.cpp
        src/performance/SIMDVectorization.cpp
        src/performance/EventQueue.cpp
        src/performance/CheckpointSystem.cpp
        src/performance/SparseConnectivity.cpp
    )
    
    target_include_directories(nlm_performance PUBLIC src/performance)
    target_link_libraries(nlm_performance PUBLIC nlm_core)
    
    # Link performance libraries to main components
    target_link_libraries(nlm_core PUBLIC nlm_performance)
endif()

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

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)

target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration experiments (main demo)
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)

target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Main executable with Phase 6 integration
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Demo executables with Phase 6 features

# Phase 3 demo (basic world interaction)
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo (emerging cognition)
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 5 demo (lifetime learning)
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)

target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Phase 6 integration demo (comprehensive test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)

target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

# Main demonstration executable
add_executable(nlm_main
    src/main.cpp
)

target_link_libraries(nlm_main PUBLIC nlm_core)

# Testing configuration
enable_testing()

# Add test directory conditionally
if(NLM_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Installation targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_main
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Export configuration for find_package support
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/NLM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)

# Package summary
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Performance optimizations: ${NLM_ENABLE_PERFORMANCE}")
message(STATUS "  Tests enabled: ${NLM_BUILD_TESTS}")
message(STATUS "  Number of core source files: ${NLM_CORE_SOURCES}")