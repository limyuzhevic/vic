cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 DESCRIPTION "Neural Learning Machine (熙然) - An Experimental Artificial Developmental Brain" LANGUAGES CXX)

# C++20 standard with compiler-specific adjustments
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(WIN32)
    set(NLM_PLATFORM "Windows")
    set(NLM_PLATFORM_WINDOWS 1)
    message(STATUS "Building for Windows platform")
elseif(APPLE)
    set(NLM_PLATFORM "macOS")
    set(NLM_PLATFORM_MACOS 1)
    message(STATUS "Building for macOS platform")
else()
    set(NLM_PLATFORM "Linux")
    set(NLM_PLATFORM_LINUX 1)
    message(STATUS "Building for Linux platform")
endif()

# Compiler detection and configuration
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(NLM_COMPILER "MSVC")
    set(NLM_COMPILER_MSVC 1)
    message(STATUS "Using MSVC compiler: ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(NLM_COMPILER "Clang")
    set(NLM_COMPILER_CLANG 1)
    message(STATUS "Using Clang compiler: ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
    # Extract clang version
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE CLANG_VERSION_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REGEX REPLACE ".*clang version ([0-9]+)\." "\\1" CLANG_VERSION "${CLANG_VERSION_OUTPUT}")
    set(NLM_CLANG_VERSION ${CLANG_VERSION})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(NLM_COMPILER "GCC")
    set(NLM_COMPILER_GCC 1)
    message(STATUS "Using GCC compiler: ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
    # Extract GCC version
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE GCC_VERSION_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REGEX REPLACE ".*gcc\(GNU\) ([0-9]+)\." "\\1" GCC_VERSION "${GCC_VERSION_OUTPUT}")
    set(NLM_GCC_VERSION ${GCC_VERSION})
else()
    set(NLM_COMPILER "Unknown")
    message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Build type with all standard options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Platform-specific compiler flags
if(MSVC)
    # MSVC specific optimization and warning flags
    add_compile_options(/W4)
    add_compile_options(/permissive-)
    add_compile_options(/DNOMINMAX)
    # SIMD optimizations for MSVC
    add_compile_options(/arch:AVX2)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /Gy /GF")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Ob0 /Od")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC and Clang specific optimization and warning flags
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
    add_compile_options(-DNOMINMAX)
    add_compile_options(-fdiagnostics-color=always)
    
    # Optimization flags for Release build
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
    
    # Position independent code for libraries
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Build options with comprehensive feature toggles
option(NLM_BUILD_TESTS "Build test executables" ON)
option(NLM_BUILD_BENCHMARKS "Build performance benchmarks" ON)
option(NLM_BUILD_OPTIMIZATION_TESTS "Build optimization verification tests" ON)
option(NLM_ENABLE_OPENMP "Enable OpenMP parallelism" ON)
option(NLM_ENABLE_SIMD "Enable SIMD optimizations" ON)
option(NLM_ENABLE_AVX2 "Enable AVX2 instruction set" ON)
option(NLM_ENABLE_AVX512 "Enable AVX512 instruction set" OFF)
option(NLM_ENABLE_SSE2 "Enable SSE2 instruction set" ON)
option(NLM_ENABLE_THREADS "Use C++ threads instead of OpenMP" OFF)
option(NLM_ENABLE_LOG_LEVEL_DEBUG "Enable debug logging" OFF)
option(NLM_BUILD_PACKAGE "Build package distribution files (RPM/DEB)" OFF)
option(NLM_INSTALL_DOC "Install documentation" ON)

# Optimization level configuration
if(NLM_ENABLE_SIMD)
    if(MSVC)
        add_compile_options(/arch:AVX2)
        if(NLM_ENABLE_AVX512)
            add_compile_options(/arch:AVX512)
        endif()
    else()
        add_compile_options(-march=native)
        if(NLM_ENABLE_AVX2)
            add_compile_options(-mavx2)
        endif()
        if(NLM_ENABLE_AVX512)
            add_compile_options(-mavx512f -mavx512dq)
        endif()
        if(NLM_ENABLE_SSE2)
            add_compile_options(-msse2)
        endif()
    endif()
endif()

# Threading model configuration
if(NLM_ENABLE_OPENMP)
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        add_compile_options(${OpenMP_CXX_FLAGS})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        set(NLM_THREADS_LIBRARIES ${OpenMP_CXX_FLAGS})
        message(STATUS "OpenMP enabled: ${OpenMP_CXX_FLAGS}")
    endif()
elseif(NLM_ENABLE_THREADS)
    find_package(Threads REQUIRED)
    set(NLM_THREADS_LIBRARIES Threads::Threads)
    message(STATUS "C++ threads enabled")
else()
    message(STATUS "No threading model enabled")
endif()

# Dependency management
find_package(Threads REQUIRED)

# Find Python for Python bindings (if needed)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/CMakeLists.txt")
    message(STATUS "Python bindings directory found - consider running python/CMakeLists.txt separately")
endif()

# Library dependencies
set(EXTERNAL_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

# System dependency checks
include(CheckSymbolExists)
include(CheckIncludeFile)

# Check for SIMD support
check_symbol_exists(__AVX2__ "xmmintrin.h" HAS_AVX2)
if(HAS_AVX2 AND NLM_ENABLE_AVX2)
    set(NLM_ARCH_FLAGS "${NLM_ARCH_FLAGS} -mavx2")
endif()

check_symbol_exists(__AVX512F__ "immintrin.h" HAS_AVX512F)
if(HAS_AVX512F AND NLM_ENABLE_AVX512)
    set(NLM_ARCH_FLAGS "${NLM_ARCH_FLAGS} -mavx512f -mavx512dq")
endif()

# Installation configuration
set(INSTALL_BINDIR "bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIBDIR "lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDEDIR "include" CACHE PATH "Installation directory for headers")
set(INSTALL_DATAROOTDIR "share" CACHE PATH "Installation directory for data files")
set(INSTALL_DOCDIR "${INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME}" CACHE PATH "Installation directory for documentation")

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

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

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

# Testing
enable_testing()
add_subdirectory(tests)

# Performance benchmark executable
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core nlm_agent nlm_world)

# Optimization verification executable  
add_executable(nlm_optimization_tests
    tests/test_optimizations.cpp
)
target_link_libraries(nlm_optimization_tests PUBLIC nlm_core nlm_agent nlm_world)

# Installation configuration
set(INSTALL_BINDIR "bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIBDIR "lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDEDIR "include" CACHE PATH "Installation directory for headers")
set(INSTALL_DATAROOTDIR "share" CACHE PATH "Installation directory for data files")
set(INSTALL_DOCDIR "${INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME}" CACHE PATH "Installation directory for documentation")

# Package content
install(DIRECTORY ${NLM_SOURCE_DIR}/ DESTINATION ${INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION ${INSTALL_LIBDIR}
    LIBRARY DESTINATION ${INSTALL_LIBDIR}
    RUNTIME DESTINATION ${INSTALL_BINDIR}
)

# Package management (RPM/DEB)
if(NLM_BUILD_PACKAGE)
    message(STATUS "Package creation enabled")
    # RPM package configuration
    if(DEFINED ENV{RPM_PACKAGER})
        set(CPACK_GENERATOR "RPM")
        set(CPACK_RPM_PACKAGE_NAME "${PROJECT_NAME}")
        set(CPACK_RPM_PACKAGE_VERSION "${PROJECT_VERSION}")
        set(CPACK_RPM_PACKAGE_RELEASE "1")
        set(CPACK_RPM_PACKAGE_DESCRIPTION "NLM - Neural Learning Machine (熙然)")
        set(CPACK_RPM_PACKAGE_LICENSE "MIT")
        set(CPACK_RPM_PACKAGE_REQUIRES "cmake >= 3.16, gcc-c++ >= 7.0")
    endif()
    
    # DEB package configuration
    if(DEFINED ENV{DEBIAN_PACKAGER})
        set(CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_NAME "${PROJECT_NAME}")
        set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
        set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_NAME} - Neural Learning Machine (熙然)")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Authors")
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "cmake, g++, python3")
    endif()
    
    include(CPack)
endif()
