# NLM CMake Configuration - Enhanced Modern CMake
# This file provides a modern, target-based CMake configuration for the NLM neural simulation framework

cmake_minimum_required(VERSION 3.22)

# Project configuration with metadata
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - A comprehensive neural simulation framework"
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
    LANGUAGES_CXX_STANDARD 20
    LANGUAGES_CXX_STANDARD_REQUIRED ON
    LANGUAGES_CXX_EXTENSIONS OFF
)

# CMake policies for modern behavior
if(POLICY CMP0074)
    # RPATH settings
    cmake_policy(SET CMP0074 NEW)
endif()

if(POLICY CMP0077)
    # Verbosity of linking
    cmake_policy(SET CMP0077 NEW)
endif()

if(POLICY CMP0091)
    # MSVC runtime library
    cmake_policy(SET CMP0091 NEW)
endif()

# Define project options
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_DEMOS "Build demo executables" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_BENCHMARKS "Enable benchmark builds" ON)
option(NLM_INSTALL_DEMOS "Install demo executables" OFF)
option(NLM_INSTALL_PYTHON_BINDINGS "Install Python bindings" ON)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Output directories for out-of-source builds
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler configuration and optimization
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific compiler flags and optimization
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
    add_definitions(-DNOMINMAX)
endif()

# Enable fast math optimizations in Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/fp:fast)
    else()
        add_compile_options(-ffast-math -march=native)
    endif()
endif()

# Position Independent Code for shared libraries
if(BUILD_SHARED_LIBS)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Coverage configuration
if(NLM_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    endif()
endif()

# Dependency management
include(FetchContent)
include(GNUInstallDirs)

# Package configuration helper
include(CMakePackageConfigHelpers)

# Find required packages
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED CONFIG)

# Configure dependency management based on available tools
include(cmake/DependencyManagement.cmake)

# Add source directory
set(NLM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Core library target
add_library(nlm_core
    STATIC
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# Add compile definitions for the core library
target_compile_definitions(nlm_core PRIVATE
    $<$<CONFIG:Debug>:DEBUG_NLM>
    $<$<CONFIG:Release>:OPTIMIZED_NLM>
)

# Agent system target
add_library(nlm_agent
    STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system target
add_library(nlm_world
    STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments target
add_library(nlm_phase3
    STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments target
add_library(nlm_phase4
    STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable target
add_executable(nlm ${NLM_SOURCE_DIR}/main.cpp)
target_link_libraries(nlm PRIVATE nlm_core)

# Phase 3 demo executable
if(NLM_BUILD_DEMOS)
    add_executable(nlm_phase3_demo ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(NLM_BUILD_DEMOS)
    add_executable(nlm_phase4_demo ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Python bindings target
if(NLM_BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Tests target
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Benchmark target
if(NLM_ENABLE_BENCHMARKS)
    add_executable(nlm_benchmark ${NLM_SOURCE_DIR}/benchmark.cpp)
    target_link_libraries(nlm_benchmark PRIVATE nlm_core)
    add_test(NAME nlm_benchmark COMMAND nlm_benchmark)
endif()

# Installation configuration
include(GNUInstallDirs)

install(TARGETS nlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(NLM_INSTALL_DEMOS AND NLM_BUILD_DEMOS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.hpp"
)

# Create package configuration files
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

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# CPack configuration for packaging
include(CPack)

set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine - Neural Simulation Framework")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Comprehensive neural simulation framework with advanced learning algorithms")
set(CPACK_PACKAGE_VENDOR "NLM Project")
set(CPACK_PACKAGE_CONTACT "nlm-authors@example.com")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

# Package dependencies based on platform
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "NLM Neural Learning Machine")
    set(CPACK_NSIS_CONTACT "nlm-authors@example.com")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_DMG_VOLUME_NAME "NLM-${PROJECT_VERSION}")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Authors")
    set(CPACK_RPM_PACKAGE_VENDOR "NLM Project")
endif()

# Package contents
cpack_include_file("README.md" README.md)
cpack_include_file("LICENSE" LICENSE)
cpack_include_file("docs/InstallationGuide.md" InstallationGuide.md)

# Export configuration for find_package
export(EXPORT NLMTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake
    NAMESPACE NLM::
)

# Add custom targets for development
add_custom_target(doc
    COMMAND doxygen Doxyfile
    COMMENT "Generate documentation"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(bindings
    COMMAND ${Python_EXECUTABLE} -c "import pybind11; print('pybind11 found')"
    COMMENT "Verify Python bindings dependencies"
)

add_custom_target(performance-benchmark
    COMMAND nlm_benchmark
    DEPENDS nlm_benchmark
    COMMENT "Run performance benchmarks"
)

message(STATUS "NLM CMake configuration complete")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "Tests: ${NLM_BUILD_TESTS}")