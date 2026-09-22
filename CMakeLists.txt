cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type with more options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Generate compile_commands.json for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable warnings with better configuration
if(MSVC)
    add_compile_options(/W4 /WX)  # Warnings as errors
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

# Optimization flags for different build types
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

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

# Position independent code for better sharing
target_compile_options(nlm_core PRIVATE -fPIC)

# Phase 2: Real neural computation components (optimized)
add_library(nlm_phase2 STATIC
    # Phase 2 specific optimizations
    src/dynamics/SpikeSystem.cpp
    src/plasticity/STDP.cpp
    src/plasticity/Hebbian.cpp
    src/brain/Brain.cpp
    src/brain/Neuron.cpp
)

target_include_directories(nlm_phase2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(nlm_phase2 PRIVATE -O2 -march=native)

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

# Phase 5 performance optimizations
add_library(nlm_phase5 STATIC
    src/performance/OptimizedBrain.cpp
    src/performance/MemoryPool.cpp
    src/performance/EventQueue.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_compile_options(nlm_phase5 PRIVATE -O3 -march=native -ftree-vectorize)

# Phase 6 integration (main executable)
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_phase2)

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

# Phase 6 enhanced demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_phase5)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets
install(TARGETS nlm nlm_core nlm_phase2 nlm_phase3 nlm_phase4 nlm_phase5
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/ DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Export configuration for downstream usage
include(GNUInstallDirs)

# Package configuration for find_package support
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Generate a compile_commands.json for tooling
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    file(WRITE ${CMAKE_BINARY_DIR}/compile_commands.json "[]")
endif()

# Add options for build configuration
option(NLM_ENABLE_TESTS "Build tests" ON)
option(NLM_ENABLE_EXPERIMENTS "Build experiment targets" ON)
option(NLM_ENABLE_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_ENABLE_VISUALIZATION "Build visualization modules" OFF)
option(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS "Enable performance optimizations" ON)
option(NLM_BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Summary output
message(STATUS "NLM Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Testing enabled: ${NLM_ENABLE_TESTS}")
message(STATUS "  Experiments enabled: ${NLM_ENABLE_EXPERIMENTS}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Visualization: ${NLM_ENABLE_VISUALIZATION}")
message(STATUS "  Performance optimizations: ${NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS}")
message(STATUS "  Shared libraries: ${NLM_BUILD_SHARED_LIBS}")

# Python bindings (optional subproject)
if(NLM_ENABLE_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Advanced features
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Features.cmake)
