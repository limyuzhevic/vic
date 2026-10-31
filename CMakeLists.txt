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

# Brain - Modular Implementation
# ===========================
# 
# The brain library is now organized into modular components for better maintainability
# and separation of concerns. Each component handles specific aspects of brain functionality.

# Core brain components (neuron and synapse models)
add_library(nlm_brain_core STATIC
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
)

# Brain implementation modules (extracted from Brain.cpp)
add_library(nlm_brain_impl STATIC
    # Initialization logic
    src/brain/Impl/BrainInitialization.cpp
    # Step logic (main simulation loop)
    src/brain/Impl/BrainStepLogic.cpp
    # Memory systems integration
    src/brain/Impl/BrainMemorySystems.cpp
    # Neuromodulation systems integration
    src/brain/Impl/BrainNeuromodulation.cpp
    # Development systems integration
    src/brain/Impl/BrainDevelopment.cpp
    # Plasticity systems integration
    src/brain/Impl/BrainPlasticity.cpp
    # Checkpointing and persistence
    src/brain/Impl/BrainCheckpointing.cpp
    # Statistics and logging
    src/brain/Impl/BrainStatistics.cpp
    # System integration and coordination
    src/brain/Impl/BrainIntegration.cpp
)

# Main Brain class interface
add_library(nlm_brain INTERFACE
    # This library provides the Brain class interface
    # The actual implementation is in nlm_brain_impl
)

# Link dependencies between brain components
target_include_directories(nlm_brain_core PUBLIC src)
target_include_directories(nlm_brain_impl PUBLIC src)
target_link_libraries(nlm_brain_impl PUBLIC nlm_core)

# Add brain components to the main core library
list(APPEND NLM_CORE_SOURCES
    # Core components (as before)
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Brain components (organized as above)
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
)

# Update the main nlm_core library to use the new structure
# Note: This requires updating the existing nlm_core target
# For now, we'll keep both approaches working

# Set include directories for brain components
target_include_directories(nlm_brain_core PUBLIC src)
target_include_directories(nlm_brain_impl PUBLIC src)

# Set C++ standard for brain components
target_compile_features(nlm_brain_core PUBLIC cxx_std_20)
target_compile_features(nlm_brain_impl PUBLIC cxx_std_20)

message(STATUS "Brain library organized into modular components")
message(STATUS "- Core brain components: nlm_brain_core")
message(STATUS "- Implementation modules: nlm_brain_impl")
message(STATUS "- Main interface: nlm_brain")


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
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)

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

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)

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

# Phase 6 demo executable
# Note: Phase 5 has no standalone demo yet - its library is wired here
# and a demo is tracked as the next cleanup step.
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Benchmark executable
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo nlm_benchmark
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
