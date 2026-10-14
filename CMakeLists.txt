cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)  # CMake 3.21+ policy for compatible interface targets
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Language Model - Advanced Neuro-Symbolic AI"
    HOMEPAGE_URL "https://github.com/neurolang/nlm"
)

# Modern C++ standards and policies
cmake_policy(SET CMP0063 NEW)  # Provide a useful export macro

# C++20 standard with explicit settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific optimizations and warnings
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # PIC for shared libraries

if(MSVC)
    add_compile_options(/W4 /permissive- /EHsc /GS /DYNAMICBASE)
    add_compile_options(/bigobj)  # Support large source files
else()
    # GCC/Clang modern warning flags
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-Werror=return-type)  # Catch implicit returns
    add_compile_options(-Werror=uninitialized)  # Warn on uninitialized variables
    add_compile_options(-Werror=null-dereference)  # Null pointer dereferences
    add_compile_options(-Werror=array-bounds)  # Array bounds checking
    add_compile_options(-Werror=sign-conversion)  # Sign conversions
    add_compile_options(-Werror=shadow)  # Shadowed variables
    add_compile_options(-Werror=pedantic)  # Strict pedantic warnings
    
    # Position independent code flags for GCC
    add_compile_options(-fPIC)
endif()

# Security hardening flags
if(NOT MSVC)
    add_compile_options(-fstack-protector-strong -fPIE -Wno-format-security)
    add_link_options(-pie -Wl,--build-id)
endif()

# Build type configurations with modern compiler optimizations
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Defaulting to Release build type")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SKIP_RPATH FALSE)

# Platform-specific configurations
if(WIN32)
    # Windows-specific configurations
    add_definitions(-DNLM_PLATFORM_WINDOWS -D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(APPLE)
    # macOS-specific configurations
    add_definitions(-DNLM_PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    # Linux/Unix-specific configurations
    add_definitions(-DNLM_PLATFORM_LINUX)
endif()

# Dependency management
find_package(nlohmann_json 3.11.2 REQUIRED)

# Optional dependencies with find_package
if(PKG_CONFIG_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OPENSSL openssl)
    if(OPENSSL_FOUND)
        add_definitions(-DNLM_HAS_OPENSSL)
        target_include_directories(nlohmann_json INTERFACE ${OPENSSL_INCLUDE_DIRS})
        target_link_libraries(nlohmann_json INTERFACE ${OPENSSL_LIBRARIES})
    endif()
endif()

# Find other potential dependencies (commented for future expansion)
# find_package(fmt REQUIRED)
# find_package(spdlog REQUIRED)
# find_package(argparse REQUIRED)

# Source directory
file(GLOB_RECURSE NLM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cxx
)

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

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

# Additional experimental libraries for future expansion
if(BUILD_PHASE5_EXPERIMENTS)
    add_library(nlm_phase5 STATIC
        src/experiments/Phase5Experiment.cpp
    )
    target_include_directories(nlm_phase5 PUBLIC src)
    target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)
endif()

if(BUILD_PHASE6_EXPERIMENTS)
    add_library(nlm_phase6 STATIC
        src/experiments/Phase6Experiment.cpp
        src/experiments/Phase6IntegratedExperiment.cpp
    )
    target_include_directories(nlm_phase6 PUBLIC src)
    target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Main executable
add_executable(nlm
    src/main.cpp
    src/benchmark.cpp
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

# Additional demo executables
if(BUILD_PHASE5_EXPERIMENTS)
    add_executable(nlm_phase5_demo
        src/experiments/Phase5Experiment.cpp
        src/experiments/Phase5Demo.cpp
    )
    target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase5)
endif()

if(BUILD_PHASE6_EXPERIMENTS)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)
endif()

# Configure testing integration
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Export configurations for package management
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Create and install the package configuration
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION lib/cmake/NLM
)

# Install configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION lib/cmake/NLM
)

# Export targets for find_package
export(EXPORT nlmTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake
    NAMESPACE NLM::
)

# Install exported targets
install(EXPORT nlmTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION lib/cmake/NLM
)

# Generate and install development package
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.hpp"
)

# Install documentation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    DESTINATION share/doc/NLM
)

# Install executables and libraries
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT nlmTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION bin
)

if(BUILD_PHASE5_EXPERIMENTS)
    install(TARGETS nlm_phase5
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
    install(TARGETS nlm_phase5_demo
        RUNTIME DESTINATION bin
    )
endif()

if(BUILD_PHASE6_EXPERIMENTS)
    install(TARGETS nlm_phase6
        EXPORT nlmTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
    install(TARGETS nlm_phase6_demo
        RUNTIME DESTINATION bin
    )
endif()

# Configuration options
option(BUILD_TESTING "Enable testing" ON)
option(BUILD_PHASE5_EXPERIMENTS "Build Phase 5 experiments" OFF)
option(BUILD_PHASE6_EXPERIMENTS "Build Phase 6 experiments" OFF)
option(BUILD_DEMOS "Build demo executables" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(USE_SYSTEM_JSON "Use system JSON library instead of bundled" OFF)
option(ENABLE_COVERAGE "Enable test coverage reporting" OFF)
option(ENABLE_SANITIZERS "Enable memory/thread sanitizers" OFF)

# Coverage and sanitizers
if(ENABLE_COVERAGE AND NOT MSVC)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

if(ENABLE_SANITIZERS AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined,thread)
    add_link_options(-fsanitize=address,undefined,thread)
endif()

# Documentation section
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/build_summary.txt
    "NLM Build Summary:\n"
    "=================\n\n"
    "Build Type: ${CMAKE_BUILD_TYPE}\n"
    "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}\n"
    "C++ Standard: ${CMAKE_CXX_STANDARD}\n\n"
    "Enabled Components:\n"
    "  - Core library\n"
    "  - Agent system\n"
    "  - World system\n"
    "  - Phase 3 experiments\n"
    "  - Phase 4 experiments\n\n"
    "Optional Components:\n"
    "  - Phase 5 experiments: ${BUILD_PHASE5_EXPERIMENTS}\n"
    "  - Phase 6 experiments: ${BUILD_PHASE6_EXPERIMENTS}\n"
    "  - Testing: ${BUILD_TESTING}\n"
    "  - Shared libraries: ${BUILD_SHARED_LIBS}\n"
)

message(STATUS "NLM Configuration Complete")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
