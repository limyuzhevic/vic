# NLM (熙然) - Neural Learning Machine CMake Configuration
# Phase 6: Final Integration Build System
# 
# Modern CMake build configuration for NLM neural simulation framework
# 
# This build system implements:
# - Modern CMake practices (target-based, folders, dependencies)
# - C++20 standard features with SIMD and parallelism
# - Comprehensive testing and validation
# - Installation and package management
# - Cross-platform compatibility
# 
# Build Types:
# - Release (optimized, no debug symbols)
# - Debug (full debugging, assertions enabled)
# - RelWithDebInfo (optimized with debug symbols)
# - MinSizeRel (minimal size optimization)
# 
# Supported Platforms:
# - Linux (GCC/Clang)
# - macOS (Clang)
# - Windows (MSVC)

cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# CMake policy settings for modern CMake behavior
cmake_policy(SET CMP0063 NEW)  # Honor visibility properties
cmake_policy(SET CMP0077 NEW)  # option() honors normal variables

cmake_policy(SET CMP0134 NEW)  # Use generator expressions in install()

# Project metadata
project(
    NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine - Experimental artificial developmental brain"
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
    LANGUAGES_CXX_STANDARD 20
    LANGUAGES_CXX_STANDARD_REQUIRED ON
    LANGUAGES_CXX_EXTENSIONS OFF
)

# Compilation options and modern C++ features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Windows MSVC specific optimizations
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS _CRT_DISABLE_PERFTRM_COUNTERS)
else()
    # GCC/Clang: Comprehensive warning flags and security
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wformat=2
        -Wconversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wlogical-op
        -Wmisleading-indentation
        -Wduplicated-branches
        -Wduplicate-method
        -Wctor-dtor-privacy
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wweak-vtables
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -fPIC
        -fvisibility=hidden
    )
    add_link_options(-Wl,--exclude-libs,libstdc++.a -Wl,--no-undefined)
endif()

# Build configuration and optimization levels
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# Set up build configurations with specific settings
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler optimization flags for different build types
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)

if(BUILD_TYPE_UPPER STREQUAL "DEBUG")
    add_compile_options(-g -O0 -DDEBUG)
    add_compile_definitions(NLM_DEBUG_MODE NDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "RELEASE")
    add_compile_options(-O3 -DNDEBUG)
    add_compile_definitions(NLM_RELEASE_MODE)
elseif(BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    add_compile_options(-O2 -g -DNDEBUG)
    add_compile_definitions(NLM_DEBUG_INFO)
elseif(BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
    add_compile_options(-Os -DNDEBUG)
    add_compile_definitions(NLM_MIN_SIZE)
endif()

# Source directory and project structure
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH "NLM source directory")
set(NLM_PYTHON_BINDINGS "${CMAKE_CURRENT_SOURCE_DIR}/python/bindings.cpp" CACHE PATH "Python bindings source")

# Configure project folders for better IDE organization
set(CMAKE_FOLDERS_SUPPORTED ON)
set(CMAKE_FOLDERS_MODE "GROUP")

# Create logical folders for source files
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/core/Types)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/core/Config)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/core/Random)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/core/Logger)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/brain)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/dynamics)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/plasticity)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/development)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/neuromodulation)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/memory)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/cognition)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/sensory)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/prediction)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/motor)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/environment)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/experiments)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/visualization)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/agent)
file(MAKE_DIRECTORY ${NLM_SOURCE_DIR}/world)

# Performance and optimization settings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Enable modern GCC/Clang optimizations
    add_compile_options(
        -march=native
        -mtune=native
        -flto
        -fomit-frame-pointer
        -freorder-blocks
        -fpeel-loops
        -fstrict-aliasing
        -falign-functions=16
        -falign-jumps=16
        -falign-labels=16
        -falign-loops=16
        -fsplit-loops-max-cost=500
        -fsplit-limits-openmp
        -fwrapv
    )
    
    # SIMD vectorization support
    add_compile_options(-mavx2 -mfma -mavx512f)
    
    # Parallel build optimizations
    set(CMAKE_JOB_POOLS "compile=0;link=0")
    set(CMAKE_JOB_POOL_COMPILE "compile")
    set(CMAKE_JOB_POOL_LINK "link")
    
elseif(MSVC)
    # MSVC specific optimizations
    add_compile_options(
        /fp:fast
        /Oi
        /Ot
        /Og
        /arch:AVX2
        /fp:except-
        /player:roi
        /plt
    )
    add_definitions(-DNOMINMAX -DFYNOQ)
endif()

# Core library with modern CMake target properties
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
    
    # Dynamics and simulation
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity rules
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
    
    # Environment interface
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiments and metrics
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization interface
    src/visualization/VisualizationInterface.cpp
)

# Core library target properties and dependencies
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_core PUBLIC cxx_std_20)

# Position-independent code for static library
if(MSVC)
    target_compile_options(nlm_core PRIVATE /DYNAMICBASE)
else()
    target_compile_options(nlm_core PRIVATE -fPIC)
endif()

# Enable link-time optimization for better performance
if(MSVC)
    target_link_options(nlm_core PRIVATE /OPT:REF /OPT:ICF)
else()
    target_link_options(nlm_core PRIVATE -flto -Wl,--gc-sections -Wl,--icf=all)
endif()

# Agent system library
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC src)
target_compile_features(nlm_agent PUBLIC cxx_std_20)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system library
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC src)
target_compile_features(nlm_world PUBLIC cxx_std_20)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiment library
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC src)
target_compile_features(nlm_phase3 PUBLIC cxx_std_20)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiment library
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC src)
target_compile_features(nlm_phase4 PUBLIC cxx_std_20)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)

target_compile_features(nlm PRIVATE cxx_std_20)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_compile_features(nlm_phase3_demo PRIVATE cxx_std_20)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_compile_features(nlm_phase4_demo PRIVATE cxx_std_20)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Python bindings executable
add_library(pynlm MODULE
    ${NLM_PYTHON_BINDINGS}
)

set_target_properties(pynlm PROPERTIES
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXT}"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(pynlm PRIVATE
    ${NLM_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/python
)

target_link_libraries(pynlm PRIVATE nlm_core)

# Python package installation
install(TARGETS pynlm
    LIBRARY DESTINATION python/pynlm
    RUNTIME DESTINATION python/pynlm
    ARCHIVE DESTINATION python/pynlm
)

# Testing configuration
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install Python module
install(FILES
    ${NLM_PYTHON_BINDINGS}
    DESTINATION python/pynlm
)

# Create configuration file for find_package
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION lib/cmake/NLM
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION lib/cmake/NLM
)

# Package summary and help
target_sources(nlm_core PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/README.md>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/LICENSE>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CONTRIBUTING.md>
)

# Print build configuration summary
function(print_build_summary)
    message(STATUS "=== NLM Build Configuration Summary ===")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "Source directory: ${NLM_SOURCE_DIR}")
    message(STATUS "Python bindings: ${NLM_PYTHON_BINDINGS}")
    message(STATUS "Core library targets: nlm_core, nlm_agent, nlm_world")
    message(STATUS "Executables: nlm, nlm_phase3_demo, nlm_phase4_demo")
    message(STATUS "Python module: pynlm")
    message(STATUS "Testing enabled: ${CMAKE_PROJECT_NAME}_test")
    message(STATUS "===============================================")
endfunction()

# Call build summary when configuring
print_build_summary()

# Export all targets for downstream projects
export(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
       FILE ${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake)

# Summary message
message(STATUS "NLM build configuration complete!")
message(STATUS "Use 'make' or 'cmake --build .' to build the project")
message(STATUS "Use 'ctest' to run tests")
message(STATUS "Install with 'cmake --install . --prefix <install_prefix>'")
