cmake_minimum_required(VERSION 3.16)

project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Language Model simulation framework"
)

# ==============================================================================
# C++ CONFIGURATION
# ==============================================================================

# C++20 standard with strict requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Position Independent Code for static linking (required for shared libraries)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ==============================================================================
# BUILD TYPE CONFIGURATION
# ==============================================================================

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Define all available build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

# Default compiler flags for different build types
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG -DOPTIMIZED")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# ==============================================================================
# PLATFORM-SPECIFIC CONFIGURATION
# ==============================================================================

# Platform detection
if(WIN32)
    set(NLM_PLATFORM_WINDOWS TRUE)
    message(STATUS "Configuring for Windows platform")
elseif(APPLE)
    set(NLM_PLATFORM_APPLE TRUE)
    message(STATUS "Configuring for Apple platform")
elseif(UNIX)
    set(NLM_PLATFORM_UNIX TRUE)
    message(STATUS "Configuring for Unix platform")
endif()

# ==============================================================================
# ENHANCED WARNING CONFIGURATION
# ==============================================================================

# Error on warnings for all compilers
if(MSVC)
    # MSVC: Treat warnings as errors
    add_compile_options(/WX)
    add_compile_options(/W4)
else()
    # GCC/Clang: Treat warnings as errors
    add_compile_options(-Werror -Wall -Wextra -Wpedantic)
endif()

# Platform-specific warning flags
if(NLM_PLATFORM_WINDOWS)
    add_compile_options(/W4 /permissive-)
elseif(NLM_PLATFORM_UNIX)
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wunused)
    if(NLM_PLATFORM_APPLE)
        add_compile_options(-Wnewline-eof)
    endif()
endif()

# ==============================================================================
# SOURCE ORGANIZATION AND LIBRARY MANAGEMENT
# ==============================================================================

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core fundamental library - basic data structures and utilities
add_library(nlm_core_fundamental
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_core_fundamental PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core>
    $<INSTALL_INTERFACE:include/core>
)

# Brain library - neural computation core
add_library(nlm_brain
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
)

target_include_directories(nlm_brain PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/brain>
    $<INSTALL_INTERFACE:include/brain>
)

# Dynamics library - neural dynamics simulation
add_library(nlm_dynamics
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
)

target_include_directories(nlm_dynamics PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/dynamics>
    $<INSTALL_INTERFACE:include/dynamics>
)

# Plasticity library - learning mechanisms
add_library(nlm_plasticity
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
)

target_include_directories(nlm_plasticity PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/plasticity>
    $<INSTALL_INTERFACE:include/plasticity>
)

# Development library - cognitive development
add_library(nlm_development
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
)

target_include_directories(nlm_development PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/development>
    $<INSTALL_INTERFACE:include/development>
)

# Neuromodulation library - reinforcement learning
add_library(nlm_neuromodulation
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
)

target_include_directories(nlm_neuromodulation PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/neuromodulation>
    $<INSTALL_INTERFACE:include/neuromodulation>
)

# Memory library - cognitive memory systems
add_library(nlm_memory
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
)

target_include_directories(nlm_memory PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/memory>
    $<INSTALL_INTERFACE:include/memory>
)

# Cognition library - higher-level cognitive functions
add_library(nlm_cognition
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
)

target_include_directories(nlm_cognition PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/cognition>
    $<INSTALL_INTERFACE:include/cognition>
)

# Sensory library - perception systems
add_library(nlm_sensory
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
)

target_include_directories(nlm_sensory PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/sensory>
    $<INSTALL_INTERFACE:include/sensory>
)

# Prediction library - forward modeling
add_library(nlm_prediction
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
)

target_include_directories(nlm_prediction PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/prediction>
    $<INSTALL_INTERFACE:include/prediction>
)

# Motor library - action selection
add_library(nlm_motor
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
)

target_include_directories(nlm_motor PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/motor>
    $<INSTALL_INTERFACE:include/motor>
)

# Environment library - world simulation
add_library(nlm_environment
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/environment/Action.cpp
)

target_include_directories(nlm_environment PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/environment>
    $<INSTALL_INTERFACE:include/environment>
)

# Experiment library - experimental utilities
add_library(nlm_experiment
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
)

target_include_directories(nlm_experiment PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/experiments>
    $<INSTALL_INTERFACE:include/experiments>
)

# Visualization library - rendering
add_library(nlm_visualization
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/visualization>
    $<INSTALL_INTERFACE:include/visualization>
)

# ==============================================================================
# MODULAR AGGREGATION LIBRARIES
# ==============================================================================

# Core library - aggregate all fundamental components
add_library(nlm_core
    nlm_core_fundamental
    nlm_brain
    nlm_dynamics
    nlm_plasticity
    nlm_development
    nlm_neuromodulation
    nlm_memory
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiment
    nlm_visualization
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Application module - high-level APIs
add_library(nlm_application STATIC
    ${NLM_SOURCE_DIR}/application/CommandLineParser.cpp
    ${NLM_SOURCE_DIR}/application/ConfigurationLoader.cpp
    ${NLM_SOURCE_DIR}/application/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/application/BrainInitializer.cpp
    ${NLM_SOURCE_DIR}/application/UserInterface.cpp
    ${NLM_SOURCE_DIR}/application/PerformanceProfiler.cpp
)

target_include_directories(nlm_application PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/application>
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/application/tests>
)

# Agent system - agent-based simulation
add_library(nlm_agent
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR}/agent)

# World system - environment and world management
add_library(nlm_world
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR}/world)

# ==============================================================================
# BUILD CONFIGURATION AND DEPENDENCIES
# ==============================================================================

# Link dependencies
target_link_libraries(nlm_core_fundamental PUBLIC nlm_core)
target_link_libraries(nlm_brain PUBLIC nlm_core)
target_link_libraries(nlm_dynamics PUBLIC nlm_core)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)
target_link_libraries(nlm_development PUBLIC nlm_core)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core)
target_link_libraries(nlm_memory PUBLIC nlm_core)
target_link_libraries(nlm_cognition PUBLIC nlm_core)
target_link_libraries(nlm_sensory PUBLIC nlm_core)
target_link_libraries(nlm_prediction PUBLIC nlm_core)
target_link_libraries(nlm_motor PUBLIC nlm_core)
target_link_libraries(nlm_environment PUBLIC nlm_core)
target_link_libraries(nlm_experiment PUBLIC nlm_core)
target_link_libraries(nlm_visualization PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp)

target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR}/experiments)

# Phase 4 experiments
add_library(nlm_phase4 STATIC ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp)

target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR}/experiments)

# ==============================================================================
# EXECUTABLE TARGETS
# ==============================================================================

# Main application executable
add_executable(nlm ${NLM_SOURCE_DIR}/main.cpp)

target_link_libraries(nlm PUBLIC nlm_core nlm_application)

# Phase 3 demo executable
add_executable(nlm_phase3_demo ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_application)

# Phase 4 demo executable
add_executable(nlm_phase4_demo ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_application)

# ==============================================================================
# TESTING CONFIGURATION
# ==============================================================================

enable_testing()

# Test discovery - automatically find test files
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

# Core functionality tests
add_executable(nlm_core_test
    tests/core_test_main.cpp
    ${TEST_SOURCES}
)

target_link_libraries(nlm_core_test PRIVATE nlm_core)

target_compile_features(nlm_core_test PRIVATE cxx_std_20)

# Application module tests
add_executable(nlm_application_test
    tests/application_test_main.cpp
)

target_link_libraries(nlm_application_test PRIVATE nlm_core nlm_application)

target_compile_features(nlm_application_test PRIVATE cxx_std_20)

# Agent system tests
add_executable(nlm_agent_test
    tests/agent_test_main.cpp
)

target_link_libraries(nlm_agent_test PRIVATE nlm_core nlm_agent)

target_compile_features(nlm_agent_test PRIVATE cxx_std_20)

# World system tests
add_executable(nlm_world_test
    tests/world_test_main.cpp
)

target_link_libraries(nlm_world_test PRIVATE nlm_core nlm_world)

target_compile_features(nlm_world_test PRIVATE cxx_std_20)

# Phase 3 experiment tests
add_executable(nlm_phase3_test
    tests/phase3_test_main.cpp
)

target_link_libraries(nlm_phase3_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3)

target_compile_features(nlm_phase3_test PRIVATE cxx_std_20)

# Phase 4 experiment tests
add_executable(nlm_phase4_test
    tests/phase4_test_main.cpp
)

target_link_libraries(nlm_phase4_test PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

target_compile_features(nlm_phase4_test PRIVATE cxx_std_20)

# Register tests with CTest
add_test(NAME nlm_core_test COMMAND nlm_core_test)
add_test(NAME nlm_application_test COMMAND nlm_application_test)
add_test(NAME nlm_agent_test COMMAND nlm_agent_test)
add_test(NAME nlm_world_test COMMAND nlm_world_test)
add_test(NAME nlm_phase3_test COMMAND nlm_phase3_test)
add_test(NAME nlm_phase4_test COMMAND nlm_phase4_test)

# ==============================================================================
# PACKAGE CONFIGURATION
# ==============================================================================

# Generate and install package configuration files
include(CMakePackageConfigHelpers)

# Create version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Create package config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

# Install package configuration
install(EXPORT nlm-targets
    FILE nlm-targets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)

# ==============================================================================
# INSTALLATION RULES
# ==============================================================================

# Install libraries
install(TARGETS nlm_core nlm_application nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT nlm-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# ==============================================================================
# DOCUMENTATION
# ==============================================================================

# Create build documentation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildOptions.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-build-options.html
    @ONLY
)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "NLM Build Configuration Complete")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")