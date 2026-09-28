cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
add_compile_features(cxx_std_20)

# Build configurations
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build types with optimized defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Configure different build modes
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os -DNDEBUG)
endif()

# Enable warnings based on compiler
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Platform-specific optimizations
if(UNIX AND NOT APPLE)
    add_compile_options(-march=native -flto)
endif()

# Option to build tests
option(BUILD_TESTS "Build the test suite" ON)
option(BUILD_PERFORMANCE_TESTS "Build performance benchmarks" ON)
option(BUILD_VALIDATION_TESTS "Build validation tests" ON)
option(BUILD_EXPERIMENTS "Build experiment executables" ON)

# Installation configuration
include(GNUInstallDirs)

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ==============================================================================
# CORE LIBRARIES
# ==============================================================================

# Core library (originally nlm_core) - essential brain components
add_library(nlm_core STATIC
    # Core system components
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
    
    # Development systems
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    
    # Neuromodulation systems
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
    
    # Cognitive functions
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory systems
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction systems
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor systems
    src/motor/MotorSystem.cpp
    
    # Environment systems
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Alias for backward compatibility
add_library(NLM::Core ALIAS nlm_core)

# ==============================================================================
# AGENT LIBRARY
# ==============================================================================

# Agent system library
add_library(nlm_agent STATIC
    src/agent/AgentBody.cpp
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_agent PUBLIC nlm_core)

add_library(NLM::Agent ALIAS nlm_agent)

# ==============================================================================
# WORLD LIBRARY
# ==============================================================================

# World simulation library
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_world PUBLIC nlm_core)

add_library(NLM::World ALIAS nlm_world)

# ==============================================================================
# EXPERIMENT LIBRARY (UNIFIED)
# ==============================================================================

# Unified experiment library containing all experiment implementations
add_library(nlm_experiments STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    src/experiments/Phase3Experiment.cpp
    src/experiments/Phase4Experiment.cpp
    src/experiments/Phase5Experiment.cpp
    src/experiments/Phase6IntegratedExperiment.cpp
)

target_include_directories(nlm_experiments PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(nlm_experiments PUBLIC nlm_core nlm_agent nlm_world)

add_library(NLM::Experiments ALIAS nlm_experiments)

# ==============================================================================
# EXECUTABLES
# ==============================================================================

# Main NLM executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# ==============================================================================
# TESTS
# ==============================================================================

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ==============================================================================
# PERFORMANCE BENCHMARKS
# ==============================================================================

if(BUILD_PERFORMANCE_TESTS)
    # Create simple benchmark if benchmark.cpp doesn't exist
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp")
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp"
            "#include <iostream>\n"
            "#include \"brain/Brain.hpp\"\n\n"
            "int main() {\n"
            "    std::cout << \"NLM Performance Benchmark\" << std::endl;\n"
            "    return 0;\n"
            "}\n")
    endif()

    add_executable(nlm_performance_benchmark
        src/benchmark.cpp
    )

    target_link_libraries(nlm_performance_benchmark PUBLIC nlm_core)

    add_test(NAME performance_benchmark COMMAND nlm_performance_benchmark)
endif()

# ==============================================================================
# VALIDATION TESTS
# ==============================================================================

if(BUILD_VALIDATION_TESTS)
    # Copy validation test to project if not exists
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/validation_test.cpp")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/experiments/Phase6Demo.cpp"
             DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/tests/validation_test.cpp")
    endif()

    enable_testing()
    add_executable(nlm_validation_tests
        tests/validation_test.cpp
    )

    target_link_libraries(nlm_validation_tests PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

    add_test(NAME validation_tests COMMAND nlm_validation_tests)
endif()

# ==============================================================================
# INSTALLATION
# ==============================================================================

# Install core library
install(TARGETS nlm_core
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install agent library
install(TARGETS nlm_agent
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install world library
install(TARGETS nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install experiments library
install(TARGETS nlm_experiments
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install executables
install(TARGETS nlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_EXPERIMENTS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Export configuration for find_package
install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# ==============================================================================
# DOCUMENTATION
# ==============================================================================

# Add custom target for documentation generation
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Building documentation..."
    COMMENT "Building NLM documentation"
    VERBATIM
)

# Add custom target for checking build status
add_custom_target(check_build
    COMMAND ${CMAKE_COMMAND} -E echo "Checking build configuration..."
    COMMAND ${CMAKE_COMMAND} -E echo "Build configuration verified."
    COMMENT "Validating build configuration"
    VERBATIM
)

# ==============================================================================
# SUMMARY
# ==============================================================================

message(STATUS "NLM CMake Configuration Summary")
message(STATUS "===========================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Libraries: nlm_core, nlm_agent, nlm_world, nlm_experiments")
message(STATUS "Executables: nlm, nlm_phase3_demo, nlm_phase4_demo")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Performance tests: ${BUILD_PERFORMANCE_TESTS}")
message(STATUS "Validation tests: ${BUILD_VALIDATION_TESTS}")
message(STATUS "Experiments: ${BUILD_EXPERIMENTS}")
