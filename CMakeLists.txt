cmake_minimum_required(VERSION 3.16)

# Project definition with modern metadata
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Experimental Artificial Developmental Brain (熙然, 'serene flow')"
    HOMEPAGE_URL "https://github.com/ EXPERIMENTAL_ARTIFICIAL_DEVELOPMENTAL_BRAIN"
    LANGUAGES_CXX_STANDARD 20
    LANGUAGES_CXX_STANDARD_REQUIRED ON
    LANGUAGES_CXX_EXTENSIONS OFF
)

# ==============================================================================
# MODERN CMAKE PRACTICES
# ==============================================================================

# 1. Policy settings for modern CMake behavior
cmake_policy(SET CMP0091 NEW)  # MSVC runtime library flags
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables

# 2. Set default build type with fallback
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# 3. Output directories for better organization
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 4. Position-independent code (PIC) - essential for shared libraries
if(NOT CMAKE_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Build position independent code" FORCE)
endif()

# ==============================================================================
# COMPILER FEATURE DETECTION AND WARNINGS
# ==============================================================================

# 1. Detect compiler features
include(CheckCXXCompilerFlag)
include(CheckIncludeFileCXX)

# 2. Advanced warning configuration
set(WARNING_FLAGS "")
set(ALL_SUPPORTED_WARNINGS "")

if(MSVC)
    # Comprehensive MSVC warnings
    add_compile_options(/W4 /permissive-)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 1920)
        add_compile_options(/WX)  # Treat warnings as errors
    endif()
else()
    # GCC/Clang warnings
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wconversion)
    
    # Additional warnings based on compiler
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(-Wlogical-op -Wduplicated-cond -Wduplicated-branches -Wnull-dereference)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-Wdocumentation -Wshadow -Werror=sign-conversion)
    endif()
    
    # Treat warnings as errors in certain configurations
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-Werror)
    endif()
endif()

# ==============================================================================
# CROSS-PLATFORM SUPPORT
# ==============================================================================

# 1. Platform-specific configurations
if(WIN32)
    message(STATUS "Building for Windows")
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)  # DLL export symbols
elseif(APPLE)
    message(STATUS "Building for macOS")
    set(CMAKE_MACOSX_RPATH ON)
elseif(UNIX)
    message(STATUS "Building for Linux/Unix")
    add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

# ==============================================================================
# DEPENDENCY DETECTION
# ==============================================================================

# 1. Find optional dependencies
find_package(Python 3.8 COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG QUIET)

# 2. JSON/YAML support detection
check_include_file_cxx("nlohmann/json.hpp" HAVE_NLOHMANN_JSON)
check_include_file_cxx("yaml-cpp/yaml.h" HAVE_YAML_CPP)

if(HAVE_NLOHMANN_JSON)
    message(STATUS "Found nlohmann/json.hpp")
else()
    message(STATUS "nlohmann/json.hpp not found - JSON features will be limited")
endif()

if(HAVE_YAML_CPP)
    message(STATUS "Found yaml-cpp/yaml.h")
else()
    message(STATUS "yaml-cpp/yaml.h not found - YAML features will be limited")
endif()

# ==============================================================================
# BUILD OPTIONS
# ==============================================================================

option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings for NLM" ON)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_EXAMPLES "Build demonstration examples" ON)
option(NLM_ENABLE_SANITIZERS "Enable memory sanitizers for debugging" OFF)
option(NLM_ENABLE_LTO "Enable link-time optimization" OFF)
option(NLM_ENABLE_SIMD "Enable SIMD intrinsics optimizations" ON)
option(NLM_ENABLE_PERF_WARNINGS "Treat performance warnings as errors" OFF)
option(NLM_INSTALL_DEVELOPER_EXTRAS "Install developer documentation and configs" ON)

# ==============================================================================
# PERFORMANCE OPTIONS
# ==============================================================================

# 1. Sanitizer configuration
if(NLM_ENABLE_SANITIZERS)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
        message(STATUS "Memory sanitizers enabled for debugging")
    endif()
endif()

# 2. Link-time optimization
if(NLM_ENABLE_LTO)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "Link-time optimization enabled")
    else()
        message(WARNING "LTO requires CMake 3.24+, skipping")
    endif()
endif()

# 3. SIMD detection and configuration
if(NLM_ENABLE_SIMD)
    # Check for SIMD capabilities
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles(""
        "__m128 x = _mm_setzero_ps();"
        HAS_SSE
    )
    if(HAS_SSE)
        add_compile_options(-msse)
        message(STATUS "SSE optimizations enabled")
    endif()
endif()

# ==============================================================================
# SOURCE FILE ORGANIZATION
# ==============================================================================

# 1. Group source files by module for better organization
set(NLM_CORE_SOURCES
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

set(NLM_BRAIN_SOURCES
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)

set(NLM_DYNAMICS_SOURCES
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

set(NLM_PLASTICITY_SOURCES
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)

set(NLM_DEVELOPMENT_SOURCES
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)

set(NLM_NEUROMODULATION_SOURCES
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)

set(NLM_MEMORY_SOURCES
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)

set(NLM_COGNITION_SOURCES
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)

set(NLM_SENSORY_SOURCES
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)

set(NLM_PREDICTION_SOURCES
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)

set(NLM_MOTOR_SOURCES
    src/motor/MotorSystem.cpp
)

set(NLM_ENVIRONMENT_SOURCES
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
)

set(NLM_EXPERIMENTS_SOURCES
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)

set(NLM_VISUALIZATION_SOURCES
    src/visualization/VisualizationInterface.cpp
)

# ==============================================================================
# LIBRARY TARGETS (Target-based approach)
# ==============================================================================

# 1. Core library - fundamental types and systems
add_library(nlm_core STATIC
    ${NLM_CORE_SOURCES}
    ${NLM_BRAIN_SOURCES}
    ${NLM_DYNAMICS_SOURCES}
    ${NLM_PLASTICITY_SOURCES}
    ${NLM_DEVELOPMENT_SOURCES}
    ${NLM_NEUROMODULATION_SOURCES}
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# 2. Agent system
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# 3. World system
add_library(nlm_world STATIC
    ${NLM_ENVIRONMENT_SOURCES}
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# 4. Memory systems
add_library(nlm_memory STATIC
    ${NLM_MEMORY_SOURCES}
)

target_include_directories(nlm_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_memory PUBLIC nlm_core)

# 5. Phase 3 experiments
add_library(nlm_phase3 STATIC
    ${NLM_EXPERIMENTS_SOURCES}
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world nlm_memory)

# 6. Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_memory)

# ==============================================================================
# EXECUTABLE TARGETS
# ==============================================================================

# 1. Main executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world nlm_memory nlm_phase3)

# 2. Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_memory nlm_phase3)

# 3. Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_memory nlm_phase3 nlm_phase4)

# ==============================================================================
# PYTHON BINDINGS
# ==============================================================================

if(NLM_BUILD_PYTHON_BINDINGS AND pybind11_FOUND)
    message(STATUS "Building Python bindings")
    add_subdirectory(python)
else()
    if(NLM_BUILD_PYTHON_BINDINGS)
        message(WARNING "pybind11 not found - Python bindings disabled")
    endif()
endif()

# ==============================================================================
# TESTS
# ==============================================================================

if(NLM_BUILD_TESTS)
    message(STATUS "Building tests")
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Tests disabled (set NLM_BUILD_TESTS=ON to enable)")
endif()

# ==============================================================================
# PACKAGE CONFIGURATION
# ==============================================================================

# 1. Generate nlm-config.cmake for package management
include(CMakePackageConfigHelpers)

# Create config file for find_package support
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

# Create version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install package configuration
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
    DESTINATION lib/cmake/nlm
)

# ==============================================================================
# INSTALLATION
# ==============================================================================

install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_memory nlm_phase3 nlm_phase4
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
install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Install development files
if(NLM_INSTALL_DEVELOPER_EXTRAS)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
        DESTINATION docs
    )
    
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/nlm-config-version.cmake
        DESTINATION lib/cmake/nlm
    )
endif()

# ==============================================================================
# DOCUMENTATION AND HELP
# ==============================================================================

# Add custom targets for documentation generation
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation build not yet configured"
    COMMENT "Generate project documentation"
)

# Status reporting at configure time
message(STATUS "================================================================================")
message(STATUS "NLM Build Configuration")
message(STATUS "================================================================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Position independent code: ${CMAKE_POSITION_INDEPENDENT_CODE}")
message(STATUS "Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "Tests: ${NLM_BUILD_TESTS}")
message(STATUS "Examples: ${NLM_BUILD_EXAMPLES}")
message(STATUS "Sanitizers: ${NLM_ENABLE_SANITIZERS}")
message(STATUS "LTO: ${NLM_ENABLE_LTO}")
message(STATUS "SIMD: ${NLM_ENABLE_SIMD}")
message(STATUS "RPATH enabled: ${CMAKE_INSTALL_RPATH}")
message(STATUS "================================================================================")
