cmake_minimum_required(VERSION 3.21)
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
    HOMEPAGE_URL "https://github.com/nlm/nlm"
)

# C++20 standard with compiler feature detection
include(CheckCXXCompilerFlag)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Modern CMake policies
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif()
if(POLICY CMP0099)
    cmake_policy(SET CMP0099 NEW)
endif()
if(POLICY CMP0102)
    cmake_policy(SET CMP0102 NEW)
endif()
if(POLICY CMP0112)
    cmake_policy(SET CMP0112 NEW)
endif()

# Build type configuration with proper defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build types" FORCE)

# Compiler feature detection and warning configuration
include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

# Function to check and add compiler flags
function(setup_target_compiler_features target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
        # Check for MSVC-specific features
        check_cxx_compiler_flag("/std:c++20" HAS_CXX20_MSVC)
        if(HAS_CXX20_MSVC)
            target_compile_options(${target_name} PRIVATE /std:c++20)
        endif()
    else()
        # GCC/Clang specific flags
        check_cxx_compiler_flag(-Wall HAS_WALL)
        if(HAS_WALL)
            target_compile_options(${target_name} PRIVATE -Wall)
        endif()
        check_cxx_compiler_flag(-Wextra HAS_WEXTRA)
        if(HAS_WEXTRA)
            target_compile_options(${target_name} PRIVATE -Wextra)
        endif()
        check_cxx_compiler_flag(-Wpedantic HAS_WPEDANTIC)
        if(HAS_WPEDANTIC)
            target_compile_options(${target_name} PRIVATE -Wpedantic)
        endif()
        check_cxx_compiler_flag(-Werror HAS_WERROR)
        if(HAS_WERROR)
            target_compile_options(${target_name} PRIVATE -Werror)
        endif()
        check_cxx_compiler_flag(-march=native HAS_MARCH_NATIVE)
        if(HAS_MARCH_NATIVE)
            target_compile_options(${target_name} PRIVATE -march=native)
        endif()
    endif()
    
    # Position independent code for shared libraries
    set_target_properties(${target_name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endfunction()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${NLM_SOURCE_DIR}")
set(NLM_DOC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")
set(NLM_CONFIG_DIR "${NLM_SOURCE_DIR}/config")

# Compiler options management
add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Compiler feature detection options
if(CMAKE_VERSION VERSION_LESS 3.24)
    message(WARNING "CMake 3.24+ recommended for better feature detection")
endif()

# Options for different build configurations
option(NLM_ENABLE_TESTS "Build NLM tests" ON)
option(NLM_ENABLE_PYTHON_BINDINGS "Build Python bindings" OFF)
option(NLM_ENABLE_EXPERIMENTS "Build experiment executables" ON)
option(NLM_BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_OPTIMIZE_PERF "Optimize for performance" OFF)

# Core library - central neural system components
add_library(nlm_core
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Neural components (central to brain function)
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Neural dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity mechanisms
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Development system
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
    src/memory/Memory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    
    # Cognitive systems
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor system
    src/motor/MotorSystem.cpp
    
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiment framework
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)
setup_target_compiler_features(nlm_core)

# Agent system - sensory perception and brain interface
add_library(nlm_agent
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)
setup_target_compiler_features(nlm_agent)

# World system - environment and interaction
add_library(nlm_world
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)
setup_target_compiler_features(nlm_world)

# Phase 3 experiments - basic brain capabilities
add_library(nlm_phase3
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
setup_target_compiler_features(nlm_phase3)

# Phase 4 experiments - cognitive capabilities
add_library(nlm_phase4
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_INCLUDE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
setup_target_compiler_features(nlm_phase4)

# Performance optimizations if requested
if(NLM_OPTIMIZE_PERF)
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -flto>
        $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL /LTCG>
    )
endif()

# Main executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)
setup_target_compiler_features(nlm)

# Phase 3 demo executable
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    setup_target_compiler_features(nlm_phase3_demo)
endif()

# Phase 4 demo executable
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    setup_target_compiler_features(nlm_phase4_demo)
endif()

# Phase 5 demo executable (Integration test)
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase5_demo
        src/experiments/Phase5Experiment.cpp
    )
    
    target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    setup_target_compiler_features(nlm_phase5_demo)
endif()

# Phase 6 demo executable (Final integration test)
if(NLM_ENABLE_EXPERIMENTS)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    setup_target_compiler_features(nlm_phase6_demo)
endif()

# Testing infrastructure
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
    
    # Add comprehensive test suite
    add_test(NAME nlm_basic_test COMMAND nlm_test)
    
    # Add integration tests for key components
    if(NLM_ENABLE_EXPERIMENTS)
        add_test(NAME nlm_phase3_demo_test COMMAND nlm_phase3_demo)
        add_test(NAME nlm_phase4_demo_test COMMAND nlm_phase4_demo)
        add_test(NAME nlm_phase5_demo_test COMMAND nlm_phase5_demo)
        add_test(NAME nlm_phase6_demo_test COMMAND nlm_phase6_demo)
    endif()
endif()

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install executables
if(NLM_ENABLE_EXPERIMENTS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo
        RUNTIME DESTINATION bin
    )
endif()

# Install headers
install(DIRECTORY ${NLM_INCLUDE_DIR}/core
    DESTINATION include/core
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/brain
    DESTINATION include/brain
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/dynamics
    DESTINATION include/dynamics
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/plasticity
    DESTINATION include/plasticity
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/development
    DESTINATION include/development
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/neuromodulation
    DESTINATION include/neuromodulation
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/memory
    DESTINATION include/memory
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/cognition
    DESTINATION include/cognition
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/sensory
    DESTINATION include/sensory
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/prediction
    DESTINATION include/prediction
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/motor
    DESTINATION include/motor
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/environment
    DESTINATION include/environment
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/agent
    DESTINATION include/agent
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/world
    DESTINATION include/world
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/experiments
    DESTINATION include/experiments
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${NLM_INCLUDE_DIR}/visualization
    DESTINATION include/visualization
    FILES_MATCHING PATTERN "*.h"
)

# Configure package for find_package support
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
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

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION lib/cmake/NLM
)