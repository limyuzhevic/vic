cmake_minimum_required(VERSION 3.19)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard with required features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Policy settings for better cross-platform compatibility
if(POLICY CMP0063)
    set(CMAKE POSITION_INDEPENDENT_CODE ON)
endif()

# Build type support with all standard configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build types" FORCE)

# Comprehensive warning levels based on build type and compiler
function(setup_target_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target_name} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target_name} PRIVATE -Werror)
        endif()
        # Additional warnings for specific compilers
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target_name} PRIVATE -Wshadow -Wnon-virtual-dtor -Wold-style-cast)
        endif()
    endif()
endfunction()

# Source directory - standardized relative path
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Build configuration options
option(NLM_ENABLE_TESTS "Enable building of tests" ON)
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_PERFORMANCE "Enable performance optimizations" OFF)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(NLM_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

# Platform-specific optimizations
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(nlm_core PRIVATE -march=native)
    if(NLM_ENABLE_PERFORMANCE)
        target_compile_options(nlm_core PRIVATE -O3 -flto)
        target_link_options(nlm_core PRIVATE -flto)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(NLM_ENABLE_PERFORMANCE)
        target_compile_options(nlm_core PRIVATE -O3 -flto)
        target_link_options(nlm_core PRIVATE -flto)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(NLM_ENABLE_PERFORMANCE)
        target_compile_options(nlm_core PRIVATE /O2 /GL)
        target_link_options(nlm_core PRIVATE /LTCG)
    endif()
endif()

# Sanitizer configurations
if(NLM_ENABLE_ASAN)
    target_compile_options(nlm_core PRIVATE -fsanitize=address)
    target_link_options(nlm_core PRIVATE -fsanitize=address)
endif()

if(NLM_ENABLE_TSAN)
    target_compile_options(nlm_core PRIVATE -fsanitize=thread)
    target_link_options(nlm_core PRIVATE -fsanitize=thread)
endif()

# Coverage configuration
if(NLM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_core PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(nlm_core PRIVATE --coverage)
endif()

# Core library - using standardized relative paths
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

# Apply warnings and include directories using standardized paths
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
setup_target_warnings(nlm_core)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)
setup_target_warnings(nlm_agent)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)
setup_target_warnings(nlm_world)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
setup_target_warnings(nlm_phase3)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
setup_target_warnings(nlm_phase4)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)
setup_target_warnings(nlm)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
setup_target_warnings(nlm_phase3_demo)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
setup_target_warnings(nlm_phase4_demo)

# Phase 6 demo executable (final integration test)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
setup_target_warnings(nlm_phase6_demo)

# Testing support
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Python bindings
if(NLM_ENABLE_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Install targets with comprehensive component support
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6_demo nlm_phase3_demo nlm_phase4_demo
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Package configuration for find_package support
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Generate and install package configuration
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
    DESTINATION lib/cmake/NLM
)

# CPack configuration for package generation
include(CPack)

# Package metadata
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine - An Experimental Artificial Developmental Brain")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NLM Neural Simulation Framework")
set(CPACK_PACKAGE_CONTACT "contact@nlm.org")
set(CPACK_PACKAGE_VENDOR "NLM Project")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")

# Dependencies
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgomp1")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc, libstdc++")

# Package components
include(CPackComponent)

cpack_component(core
    DISPLAY_NAME "Core Library"
    DESCRIPTION "Core neural simulation library"
    REQUIRED
)

cpack_component(python_bindings
    DISPLAY_NAME "Python Bindings"
    DESCRIPTION "Python bindings for NLM"
    OPTIONAL
)

cpack_component(documentation
    DISPLAY_NAME "Documentation"
    DESCRIPTION "Documentation and examples"
    OPTIONAL
)

# Component installation
cp_pack_add_target(comp_core nlm_core)
cp_pack_add_target(comp_python_bindings pynlm)

# Set component dependencies
cp_pack_component(comp_core)
cp_pack_component(comp_python_bindings)

# Package formats
if(CPACK_GENERATOR MATCHES "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Developers <contact@nlm.org>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
endif()

if(CPACK_GENERATOR MATCHES "RPM")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
endif()

if(CPACK_GENERATOR MATCHES "TGZ")
    set(CPACK_GENERATOR "TGZ")
endif()
