# NLM Build System Improvements
# Phase 2: Real Neural Computation
#
# This file contains build system improvements for NLM, including:
# - Enhanced CMake configuration
# - Installation targets
# - Configuration options
# - Test suite configuration
# - Python bindings build
# - Documentation generation
# - Package creation
#

# Minimum CMake version
cmake_minimum_required(VERSION 3.16)

# Project information
project(NLM
    VERSION 2.0.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Phase 2: Real Neural Computation"
    HOMEPAGE_URL "https://nlm.org"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build configuration options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTING "Build test suite" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_EXAMPLES "Build example executables" ON)
option(ENABLE_PERFORMANCE_OPTIMIZATIONS "Enable performance optimizations" ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(ENABLE_ADDRESS_SANITIZER "Enable address sanitizer" OFF)
option(ENABLE_UNDEFINED_BEHAVIOR_SANITIZER "Enable undefined behavior sanitizer" OFF)

# Compiler options
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-DNOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_definitions(-D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE)
endif()

# Performance optimizations
if(ENABLE_PERFORMANCE_OPTIMIZATIONS)
    if(MSVC)
        add_compile_options(/O2 /fp:fast /arch:AVX2)
    else()
        add_compile_options(-O3 -march=native -mtune=native -flto)
        add_link_options(-flto)
    endif()
endif()

# Sanitizers
if(ENABLE_ADDRESS_SANITIZER)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_UNDEFINED_BEHAVIOR_SANITIZER)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

# Coverage reporting
if(ENABLE_COVERAGE AND NOT MSVC)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
endif()

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Installation directories
set(NLM_INCLUDE_INSTALL_DIR "include/nlm" CACHE PATH "Installation directory for headers")
set(NLM_LIB_INSTALL_DIR "lib" CACHE PATH "Installation directory for libraries")
set(NLM_BIN_INSTALL_DIR "bin" CACHE PATH "Installation directory for executables")
set(NLM_PYTHON_INSTALL_DIR "${PYTHON_INSTALL_DIR}/site-packages" CACHE PATH "Installation directory for Python bindings")

# Find dependencies
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

if(BUILD_PYTHON_BINDINGS)
    find_package(pybind11 REQUIRED)
endif()

# Core library configuration
add_library(nlm_core STATIC
    # Core modules
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    
    # Brain modules
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    
    # Dynamics modules
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    
    # Plasticity modules
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    
    # Development modules
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    
    # Neuromodulation modules
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    
    # Memory modules
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    
    # Cognition modules
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    
    # Sensory modules
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    
    # Prediction modules
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    
    # Motor modules
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/motor/Action.cpp
    
    # Environment modules
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    
    # Experiment modules
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
    
    # Visualization modules
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Core library target properties
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${NLM_INCLUDE_INSTALL_DIR}>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# Agent system
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Main executable
add_executable(nlm
    ${NLM_SOURCE_DIR}/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Enhanced main executable with CLI
add_executable(nlm_enhanced
    ${NLM_SOURCE_DIR}/main.cpp
)

target_link_libraries(nlm_enhanced PUBLIC nlm_core)

# Performance benchmark executable
add_executable(nlm_benchmark
    ${NLM_SOURCE_DIR}/benchmark.cpp
)

target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    pybind11_add_module(pynlm
        ${NLM_SOURCE_DIR}/python/bindings.cpp
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core)
    
    # Set Python module properties
    set_target_properties(pynlm PROPERTIES
        OUTPUT_NAME "nlm"
        PREFIX ""
        SUFFIX ".so"
    )
endif()

# Test suite
if(BUILD_TESTING)
    enable_testing()
    
    # Add test subdirectories
    add_subdirectory(tests)
    
    # Add benchmark tests
    add_executable(nlm_benchmark_test
        ${NLM_SOURCE_DIR}/benchmark.cpp
    )
    
    target_link_libraries(nlm_benchmark_test PUBLIC nlm_core)
    add_test(NAME nlm_benchmark_test COMMAND nlm_benchmark_test)
endif()

# Documentation generation
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    
    set(DOXYGEN_INPUT ${NLM_SOURCE_DIR})
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
    
    doxygen_add_docs(docs
        ${DOXYGEN_INPUT}
        OUTPUT_DIR ${DOXYGEN_OUTPUT_DIR}
        QUIET
        NO_IMPLICIT_INCLUDES
    )
endif()

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION ${NLM_LIB_INSTALL_DIR}
    LIBRARY DESTINATION ${NLM_LIB_INSTALL_DIR}
    RUNTIME DESTINATION ${NLM_BIN_INSTALL_DIR}
)

if(BUILD_PYTHON_BINDINGS)
    install(TARGETS pynlm
        LIBRARY DESTINATION ${NLM_PYTHON_INSTALL_DIR}
    )
endif()

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/nlm/
    DESTINATION ${NLM_INCLUDE_INSTALL_DIR}
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Install configuration files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/enhanced.cfg
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/default.cfg
    DESTINATION ${NLM_INCLUDE_INSTALL_DIR}/configs
)

# Install documentation
if(BUILD_DOCS)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
        DESTINATION doc
    )
endif()

# Installation script
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/install.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/install.cmake
    @ONLY
)

# Package configuration
include(GNUInstallDirs)

# Create package config files
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Export targets for package
export(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    FILE ${CMAKE_CURRENT_BINARY_DIR}/nlmTargets.cmake
)

# Package installation
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

install(EXPORT nlmTargets
    FILE nlmTargets.cmake
    NAMESPACE nlm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Build summary
function(print_build_summary)
    message(STATUS "\n=== NLM Build Summary ===")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "Build options:")
    message(STATUS "  Shared libraries: ${BUILD_SHARED_LIBS}")
    message(STATUS "  Testing: ${BUILD_TESTING}")
    message(STATUS "  Python bindings: ${BUILD_PYTHON_BINDINGS}")
    message(STATUS "  Documentation: ${BUILD_DOCS}")
    message(STATUS "  Examples: ${BUILD_EXAMPLES}")
    message(STATUS "  Performance optimizations: ${ENABLE_PERFORMANCE_OPTIMIZATIONS}")
    message(STATUS "  Address sanitizer: ${ENABLE_ADDRESS_SANITIZER}")
    message(STATUS "  Undefined behavior sanitizer: ${ENABLE_UNDEFINED_BEHAVIOR_SANITIZER}")
    message(STATUS "  Coverage reporting: ${ENABLE_COVERAGE}")
    
    message(STATUS "\nTargets:")
    message(STATUS "  Executables:")
    message(STATUS "    nlm - Main neural simulation executable")
    message(STATUS "    nlm_enhanced - Enhanced neural simulation with CLI")
    message(STATUS "    nlm_benchmark - Performance benchmarking tool")
    message(STATUS "    nlm_phase3_demo - Phase 3 demonstration")
    message(STATUS "    nlm_phase4_demo - Phase 4 demonstration")
    message(STATUS "    nlm_benchmark_test - Benchmark test suite")
    
    message(STATUS "  Libraries:")
    message(STATUS "    nlm_core - Core neural computation library")
    message(STATUS "    nlm_agent - Agent system library")
    message(STATUS "    nlm_world - World system library")
    message(STATUS "    nlm_phase3 - Phase 3 experiments library")
    message(STATUS "    nlm_phase4 - Phase 4 experiments library")
    
    if(BUILD_PYTHON_BINDINGS)
        message(STATUS "  Python bindings: pynlm")
    endif()
    
    message(STATUS "\nInstallation directories:")
    message(STATUS "  Headers: ${NLM_INCLUDE_INSTALL_DIR}")
    message(STATUS "  Libraries: ${NLM_LIB_INSTALL_DIR}")
    message(STATUS "  Binaries: ${NLM_BIN_INSTALL_DIR}")
    
    if(BUILD_PYTHON_BINDINGS)
        message(STATUS "  Python: ${NLM_PYTHON_INSTALL_DIR}")
    endif()
    
    message(STATUS "======================\n")
endfunction()

# Add build summary to all targets
foreach(target nlm nlm_enhanced nlm_benchmark nlm_phase3_demo nlm_phase4_demo)
    if(TARGET ${target})
        add_custom_target(build_summary_${target}
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/print_build_summary.cmake
            COMMENT "Printing build summary for ${target}"
            VERBATIM
        )
    endif()
endforeach()

# Add benchmark to all targets
if(BUILD_TESTING)
    add_custom_target(run_benchmarks
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/nlm_benchmark
        COMMENT "Running performance benchmarks"
        VERBATIM
    )
endif()

# Add documentation target
if(BUILD_DOCS)
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Building documentation..."
        COMMAND doxygen
        COMMENT "Building documentation"
        VERBATIM
    )
endif()

# Configure CMake for packaging
include(CMakePackageConfigHelpers)

# Create a config file for FindNLM.cmake
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/FindNLM.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/FindNLM.cmake
    @ONLY
)

# Install FindNLM.cmake for find_package support
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/FindNLM.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Summary
print_build_summary()
