cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type and optimization flags
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Compiler detection and optimization flags
if(MSVC)
    # Microsoft Visual C++ specific optimizations
    add_compile_options(/W4)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_compile_options(/O2 /fp:fast /arch:AVX2)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        add_compile_options(/O1 /fp:fast /GS /arch:AVX2)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
        add_compile_options(/O1 /Gy /GF /fp:fast /arch:AVX2)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_compile_options(/Od /Zi /debug /arch:AVX2)
    endif()
else()
    # GCC and Clang optimizations
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_compile_options(-O3 -march=native -mtune=native -flto -ffast-math -fomit-frame-pointer)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
        add_compile_options(-O2 -g -march=native -flto -ffast-math)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
        add_compile_options(-Os -march=native -flto -ffast-math -DNDEBUG)
    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_compile_options(-O0 -g -ggdb -DDEBUG)
    endif()
endif()

# Architecture-specific optimizations
add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang>:-mfpmath=sse>)

# Position Independent Code (PIC) for shared libraries
add_compile_options(-fPIC)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

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
    src/neuromodulation/Dopamine.cpp
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

# Phase 6 experiments (integrated brain systems)
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

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

# Phase 6 demo executable (Phase 6 integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

# Testing
enable_testing()
add_subdirectory(tests)

# Enhanced install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    EXPORT nlmTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".*" EXCLUDE  # This excludes CMakeLists.txt files
)

# Package configuration
include(CMakePackageConfigHelpers)

# Generate and install package configuration
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create a config file for finding the targets
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

# Install package configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    DESTINATION lib/cmake/nlm
)

install(EXPORT nlmTargets
    FILE nlmTargets.cmake
    DESTINATION lib/cmake/nlm
)

# Documentation installation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/HOW_TO_USE.md
    DESTINATION doc
)

# Python bindings installation
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/python/CMakeLists.txt)
    enable_language(Python)
    add_subdirectory(python)
    install(EXPORT pythonTargets
        FILE pynlmTargets.cmake
        DESTINATION lib/cmake/pynlm
    )
endif()

# Summary
message(STATUS "Build configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Optimization flags: enabled")
message(STATUS "  Warning levels: enabled")
message(STATUS "  Install targets: configured")
message(STATUS "  Packaging: enabled")
