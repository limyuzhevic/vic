# Enhanced CMake Configuration for NLM

# Enhanced CMake Configuration for NLM
# 
# This file provides enhanced CMake configuration for the NLM (Neural Learning Machine)
# project, with improved build options, better dependency management, and enhanced
# functionality for both development and production builds.
#
# Key Improvements:
# 1. Better dependency management and version checking
# 2. Enhanced build configurations for different use cases
# 3. Improved installation and packaging
# 4. Better testing support
# 5. Enhanced compiler and platform support
# 6. Build time optimizations
# 7. Installation and configuration for Python bindings
#

# CMake minimum version
cmake_minimum_required(VERSION 3.16)

# Project configuration
project(NLM VERSION 0.2.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable all warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Build type configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# Enhanced build configurations
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Build tests" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_PYTHON_ENHANCED_BINDINGS "Build enhanced Python bindings" ON)
option(BUILD_DEMOS "Build demonstration executables" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Performance optimization options
option(OPTIMIZE_FOR_PERFORMANCE "Optimize for performance" ON)
option(ENABLE_VECTORIZATION "Enable SIMD vectorization" ON)
option(ENABLE_PARALLEL "Enable parallel processing" ON)
option(ENABLE_MEMORY_POOLING "Enable memory pooling" ON)

# Installation options
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix")
set(INSTALL_PYTHON_PACKAGE "ON" CACHE BOOL "Install Python package")

# Find dependencies
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Python bindings configuration
if(BUILD_PYTHON_BINDINGS)
    # Create Python module target
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core)
    target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/python)
    
    # Set Python module properties
    set_target_properties(pynlm PROPERTIES
        OUTPUT_NAME "pynlm"
        PREFIX ""
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )
    
    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION "${Python_SITEARCH}"
    )
endif()

if(BUILD_PYTHON_ENHANCED_BINDINGS)
    # Create enhanced Python module target
    pybind11_add_module(pynlm_enhanced python/bindings_enhanced.cpp)
    target_link_libraries(pynlm_enhanced PRIVATE nlm_core)
    target_include_directories(pynlm_enhanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/python)
    
    # Set enhanced Python module properties
    set_target_properties(pynlm_enhanced PROPERTIES
        OUTPUT_NAME "pynlm_enhanced"
        PREFIX ""
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )
    
    # Install enhanced Python module
    install(TARGETS pynlm_enhanced
        LIBRARY DESTINATION "${Python_SITEARCH}"
    )
endif()

# Core library configuration
add_library(nlm_core STATIC
    # Core components
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Neural components
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Dynamics components
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity components
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Development components
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    
    # Neuromodulation components
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    
    # Memory components
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    
    # Cognition components
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory components
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction components
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor components
    src/motor/MotorSystem.cpp
    
    # Environment components
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiment components
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization components
    src/visualization/VisualizationInterface.cpp
)

# Performance optimizations
if(OPTIMIZE_FOR_PERFORMANCE)
    target_compile_options(nlm_core PRIVATE -O3 -march=native)
    if(ENABLE_VECTORIZATION)
        target_compile_options(nlm_core PRIVATE -mavx -mavx2)
    endif()
    if(ENABLE_PARALLEL)
        target_compile_options(nlm_core PRIVATE -fopenmp)
        target_link_libraries(nlm_core PRIVATE omp)
    endif()
endif()

if(ENABLE_MEMORY_POOLING)
    target_compile_definitions(nlm_core PRIVATE -DENABLE_MEMORY_POOLING)
endif()

# Agent system
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
    src/agent/AgentBody.cpp
)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Demo executables
if(BUILD_DEMOS)
    # Phase 3 demo
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    
    # Phase 4 demo
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    
    # Phase 6 demo
    add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6)
endif()

# Phase libraries
add_library(nlm_phase3 STATIC src/experiments/Phase3Experiment.cpp)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

add_library(nlm_phase4 STATIC src/experiments/Phase4Experiment.cpp)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

add_library(nlm_phase6 STATIC src/experiments/Phase6IntegratedExperiment.cpp)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)

# Testing
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install Python bindings
if(INSTALL_PYTHON_PACKAGE)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml
        ${CMAKE_CURRENT_SOURCE_DIR}/python/nlm_cli.py
        ${CMAKE_CURRENT_SOURCE_DIR}/python/advanced_examples.py
        ${CMAKE_CURRENT_SOURCE_DIR}/python/ENHANCED_BINDINGS.md
        DESTINATION lib/python/nlm
    )
endif()

# Generate export configuration
install(EXPORT NLMTargets
    FILE NLMConfig.cmake
    NAMESPACE NLM::
    DESTINATION lib/cmake/NLM
)

# Create config file for development
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/nlm_config.cfg.in
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/nlm_config.cfg
    @ONLY
)

# Create compile_commands.json for tooling
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Add custom targets
add_custom_target(check-deps
    COMMAND ${CMAKE_COMMAND} -E echo "Checking dependencies..."
    COMMAND ${Python3_EXECUTABLE} -m pip list | grep -E "(pybind11|scikit-build-core)" || echo "Python dependencies not installed"
    COMMENT "Check Python dependencies"
)

add_custom_target(build-python
    COMMAND ${Python3_EXECUTABLE} -m pip install -e .
    COMMENT "Build Python package"
)

add_custom_target(run-tests
    COMMAND ctest --output-on-failure
    DEPENDS nlm_test
    COMMENT "Run all tests"
)

# Summary
message(STATUS "NLM Build Configuration")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "  Build tests: ${BUILD_TESTING}")
message(STATUS "  Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Build enhanced Python bindings: ${BUILD_PYTHON_ENHANCED_BINDINGS}")
message(STATUS "  Build demos: ${BUILD_DEMOS}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Python executable: ${Python3_EXECUTABLE}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")

# Build options summary
message(STATUS "Build options:")
message(STATUS "  Optimize for performance: ${OPTIMIZE_FOR_PERFORMANCE}")
message(STATUS "  Enable vectorization: ${ENABLE_VECTORIZATION}")
message(STATUS "  Enable parallel processing: ${ENABLE_PARALLEL}")
message(STATUS "  Enable memory pooling: ${ENABLE_MEMORY_POOLING}")