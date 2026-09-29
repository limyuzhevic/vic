cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Experimental Artificial Developmental Brain"
)

# Set project properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configure build types
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Option for development features
option(NLM_ENABLE_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_DEMOS "Enable demo executables" ON)
option(NLM_ENABLE_EXPERIMENTS "Enable experiment libraries" ON)
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_JSON_CONFIG "Enable JSON configuration support" ON)

# Platform-specific settings
if(WIN32)
    add_compile_options(/W4 /WX)
    add_definitions(-DNLM_PLATFORM_WINDOWS)
elseif(APPLE)
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_definitions(-DNLM_PLATFORM_MACOS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_definitions(-DNLM_PLATFORM_LINUX)
endif()

# Installation directories
include(GNUInstallDirs)
set(NLM_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/NLM)

# Package configuration
include(CMakePackageConfigHelpers)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ==============================================================================
# DEPENDENCY MANAGEMENT
# ==============================================================================

# Find required packages
find_package(pybind11 REQUIRED)
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Platform-specific dependencies
if(WIN32)
    # Windows-specific dependencies
else()
    # Unix-like dependencies
    find_package(Threads REQUIRED)
endif()

# ==============================================================================
# LIBRARY TARGETS
# ==============================================================================

# Core library - fundamental neural computation components
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
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
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
    
    # Prediction and planning
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor system
    src/motor/MotorSystem.cpp
    
    # Environment interface
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

# Core library includes
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# Compile options for core library
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Agent system - agent-world interface
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
    src/agent/AgentBody.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system - environment and simulation
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/Observation.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# ==============================================================================
# EXPERIMENT LIBRARIES
# ==============================================================================

if(NLM_ENABLE_EXPERIMENTS)
    # Phase 3 experiment library
    add_library(nlm_phase3 STATIC
        src/experiments/Phase3Experiment.cpp
        src/experiments/Phase3Demo.cpp
    )
    target_include_directories(nlm_phase3 PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    )
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
    
    # Phase 4 experiment library
    add_library(nlm_phase4 STATIC
        src/experiments/Phase4Experiment.cpp
        src/experiments/Phase4Demo.cpp
        src/experiments/Phase6IntegratedExperiment.cpp
    )
    target_include_directories(nlm_phase4 PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    )
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
    
    # Phase 6 experiment library
    add_library(nlm_phase6 STATIC
        src/experiments/Phase6Demo.cpp
        src/experiments/Phase6IntegratedExperiment.cpp
        src/experiments/Phase4Experiment.cpp
    )
    target_include_directories(nlm_phase6 PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    )
    target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# ==============================================================================
# EXECUTABLE TARGETS
# ==============================================================================

# Main NLM executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
if(NLM_ENABLE_DEMOS)
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(NLM_ENABLE_DEMOS)
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Phase 6 demo executable
if(NLM_ENABLE_DEMOS)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6)
endif()

# ==============================================================================
# PYTHON BINDINGS
# ==============================================================================

if(NLM_ENABLE_PYTHON_BINDINGS)
    # Python bindings CMakeLists.txt
    add_subdirectory(python)
endif()

# ==============================================================================
# TESTING
# ==============================================================================

if(NLM_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ==============================================================================
# INSTALLATION
# ==============================================================================

include(GNUInstallRules)

# Install libraries
install(TARGETS nlm_core
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*/.svn*" EXCLUDE
)

# Install executables
install(TARGETS nlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(NLM_ENABLE_DEMOS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Install CMake config
install(EXPORT NLMTargets
    FILE NLMConfig.cmake
    DESTINATION ${NLM_INSTALL_CMAKE_DIR}
)

include(CMakePackageConfigHelpers)

# Generate and install package config
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION ${NLM_INSTALL_CMAKE_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${NLM_INSTALL_CMAKE_DIR}
)

# ==============================================================================
# DOCUMENTATION AND METADATA
# ==============================================================================

# Create build information file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/nlm_build_info.txt
    "NLM Build Information\n"
    "======================\n\n"
    "Project: ${PROJECT_NAME}\n"
    "Version: ${PROJECT_VERSION}\n"
    "Build Type: ${CMAKE_BUILD_TYPE}\n"
    "Compiler: ${CMAKE_CXX_COMPILER_ID}\n"
    "C++ Standard: ${CMAKE_CXX_STANDARD}\n"
    "Install Prefix: ${CMAKE_INSTALL_PREFIX}\n\n"
    "Components:\n"
    "  - Core neural computation (nlm_core)\n"
    "  - Agent interface (nlm_agent)\n"
    "  - World simulation (nlm_world)\n"
    "  - Python bindings (if enabled)\n"
    "  - Unit tests (if enabled)\n"
    "  - Demo executables (if enabled)\n"
)

# Add custom targets
add_custom_target(info
    COMMAND echo "=== NLM Build Information ===" && cat ${CMAKE_CURRENT_BINARY_DIR}/nlm_build_info.txt
    COMMENT "Display build information"
    VERBATIM
)

add_custom_target(configure
    COMMAND ${CMAKE_COMMAND} -E echo "Configuring NLM..."
    COMMAND cmake -P ${CMAKE_CURRENT_SOURCE_DIR}/configure.cmake
    COMMENT "Configure NLM project"
    VERBATIM
)

# ==============================================================================
# SUMMARY
# ==============================================================================

message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Unit tests: ${NLM_ENABLE_TESTS}")
message(STATUS "  Demo executables: ${NLM_ENABLE_DEMOS}")
message(STATUS "  Experiment libraries: ${NLM_ENABLE_EXPERIMENTS}")
message(STATUS "  JSON config: ${NLM_ENABLE_JSON_CONFIG}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
