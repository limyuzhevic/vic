# Enhanced CMake Build System for NLM (Neural Learning Machine)
# Modern CMake implementation with enterprise-grade build capabilities

cmake_minimum_required(VERSION 3.19)
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "C++ neural network simulator with Python bindings"
    HOMEPAGE_URL "https://github.com/NLM-project/NLM"
)

# Include CMake modules for modern features
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FetchContent)

# ==============================================================================
# ADVANCED BUILD OPTIONS
# ==============================================================================

# Default build options (can be overridden by command line)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXPERIMENTS "Build experiment executables" ON)
option(ENABLE_SANITIZERS "Enable debug sanitizers (Address, Undefined)" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)
option(ENABLE_PROFILING "Enable memory profiling support" OFF)
option(USE_OPENMP "Enable OpenMP threading" OFF)
option(USE_TBB "Enable Intel TBB threading" OFF)
option(BUILD_DOCUMENTATION "Enable automatic documentation generation" OFF)

# Optimization and build configuration
set(OPTIMIZATION_LEVEL "fast" CACHE STRING "Optimization level: fast, debug, research")
set(BUILD_TYPE "Development" CACHE STRING "Build type: Development, Production, Research")
set(INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Custom installation prefix")

# ==============================================================================
# MODERN CMAKE PRACTICES
# ==============================================================================

# Use target-based approach for all configuration
# No global commands (include_directories, add_compile_options, etc.)

# Define project namespace and properties
set(NLM_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(NLM_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(NLM_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Set default C++ standard with compiler detection
if(CMAKE_CXX_STANDARD LESS 20)
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for tooling support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create a configuration package for find_package support
include(CMakePackageConfigHelpers)
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

# ==============================================================================
# DEPENDENCY MANAGEMENT
# ==============================================================================

# Platform-specific dependency detection
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(APPLE)
    set(MACOS TRUE)
endif()

if(WIN32)
    set(WINDOWS TRUE)
endif()

# Find and configure BLAS/LAPACK
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# Python and pybind11 configuration
if(BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Threading library detection
if(USE_OPENMP)
    find_package(OpenMP REQUIRED)
endif()

if(USE_TBB)
    find_package(TBB REQUIRED)
endif()

# ==============================================================================
# COMPILER AND PLATFORM CONFIGURATION
# ==============================================================================

# Function to apply platform-specific compiler flags and configurations
target_compile_features(nlm_core INTERFACE cxx_std_20)

function(apply_platform_flags target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /permissive-)
        target_compile_options(${target} PRIVATE /W4)
        target_link_options(${target} PRIVATE /WX)
    else()
        # GCC and Clang common flags
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        
        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86|amd64")
            # x86-64 specific optimizations
            target_compile_options(${target} PRIVATE -march=x86-64-v3)
            target_link_options(${target} PRIVATE -march=x86-64-v3)
        endif()
    endif()
    
    # Sanitizers
    if(ENABLE_SANITIZERS)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target} PRIVATE -fsanitize=address,undefined)
            target_link_options(${target} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
    
    # Coverage
    if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${target} PRIVATE --coverage)
    endif()
    
    # Profiling
    if(ENABLE_PROFILING)
        if(UNIX)
            target_compile_options(${target} PRIVATE -g -pg)
            target_link_options(${target} PRIVATE -g -pg)
        endif()
    endif()
endfunction()

# ==============================================================================
# BUILD TYPE IMPROVEMENTS
# ==============================================================================

# Custom build configurations
if(NOT CMAKE_BUILD_TYPE)
    if(${BUILD_TYPE} STREQUAL "Production")
        set(CMAKE_BUILD_TYPE Release)
    elseif(${BUILD_TYPE} STREQUAL "Research")
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    else()
        set(CMAKE_BUILD_TYPE Debug)
    endif()
endif()

# Optimization levels
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
if(${OPTIMIZATION_LEVEL} STREQUAL "fast")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
elseif(${OPTIMIZATION_LEVEL} STREQUAL "research")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g -march=native -fno-inline-functions-called-once")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -DDEBUG")
endif()

# ==============================================================================
# CORE LIBRARY
# ==============================================================================

# Source files organized by subsystem
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

set(NLM_SENORY_SOURCES
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
    src/visualization/VisualizationManager.cpp
    src/visualization/WebDashboard.cpp
    src/visualization/AdvancedAnalysis.cpp
    src/visualization/PerformanceMonitor.cpp
)

set(NLM_VISUALIZATION_SOURCES
    src/visualization/VisualizationInterface.cpp
    src/visualization/VisualizationManager.cpp
    src/visualization/WebDashboard.cpp
    src/visualization/AdvancedAnalysis.cpp
    src/visualization/PerformanceMonitor.cpp
)

add_library(nlm_visualization ${NLM_VISUALIZATION_SOURCES})
target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_visualization PUBLIC nlm_core)
apply_platform_flags(nlm_visualization)

# Update core library to include visualization sources
list(APPEND NLM_CORE_SOURCES ${NLM_VISUALIZATION_SOURCES})

# Recreate core library with all sources
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(nlm_core
    PUBLIC
        ${BLAS_LIBRARIES}
        ${LAPACK_LIBRARIES}
)

if(USE_OPENMP)
    target_link_libraries(nlm_core PUBLIC OpenMP::OpenMP_CXX)
endif()

if(USE_TBB)
    target_link_libraries(nlm_core PUBLIC TBB::TBB)
endif()

target_include_directories(nlm_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(nlm_core
    PUBLIC
        ${BLAS_LIBRARIES}
        ${LAPACK_LIBRARIES}
)

if(USE_OPENMP)
    target_link_libraries(nlm_core PUBLIC OpenMP::OpenMP_CXX)
endif()

if(USE_TBB)
    target_link_libraries(nlm_core PUBLIC TBB::TBB)
endif()

apply_platform_flags(nlm_core)

# ==============================================================================
# SUBSYSTEM LIBRARIES
# ==============================================================================

# Agent system
add_library(nlm_agent STATIC src/agent/AgentBrain.cpp src/agent/SensoryPercept.cpp)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)
apply_platform_flags(nlm_agent)

# World system
add_library(nlm_world STATIC src/world/SimpleWorld.cpp)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)
apply_platform_flags(nlm_world)

# Phase 3 experiments
add_library(nlm_phase3 STATIC src/experiments/Phase3Experiment.cpp)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
apply_platform_flags(nlm_phase3)

# Phase 4 experiments
add_library(nlm_phase4 STATIC src/experiments/Phase4Experiment.cpp)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
apply_platform_flags(nlm_phase4)

# ==============================================================================
# EXECUTABLES
# ==============================================================================

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core)
apply_platform_flags(nlm)

# Phase 3 demo executable
if(BUILD_EXPERIMENTS)
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    apply_platform_flags(nlm_phase3_demo)
endif()

# Phase 4 demo executable
if(BUILD_EXPERIMENTS)
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    apply_platform_flags(nlm_phase4_demo)
endif()

# ==============================================================================
# PYTHON BINDINGS
# ==============================================================================

if(BUILD_PYTHON_BINDINGS)
    # Python bindings CMakeLists.txt
    add_subdirectory(python)
endif()

# ==============================================================================
# TESTING
# ==============================================================================

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ==============================================================================
# INSTALLATION
# ==============================================================================

# Installation targets for libraries
install(TARGETS nlm_core
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Installation targets for executables
install(TARGETS nlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_EXPERIMENTS)
    install(TARGETS nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Installation of headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Installation of Python bindings
if(BUILD_PYTHON_BINDINGS)
    install(TARGETS pynlm
        LIBRARY DESTINATION ${Python_SITEARCH}/nlm
        ARCHIVE DESTINATION ${Python_SITEARCH}/nlm
        RUNTIME DESTINATION ${Python_SITEARCH}/nlm
    )
endif()

# Installation of configuration files
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

# ==============================================================================
# DOCUMENTATION AND ANALYSIS
# ==============================================================================

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
        doxygen_add_docs(docs
            src/
            COMMENT "Generating API documentation"
        )
    endif()
endif()

# Build analysis and statistics
function(generate_build_info)
    set(BUILD_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_info.json")
    file(WRITE ${BUILD_INFO_FILE} "{")
    file(APPEND ${BUILD_INFO_FILE} "\\\"project\\\":\\\"NLM\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"version\\\":\\\"${PROJECT_VERSION}\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_type\\\":\\\"${CMAKE_BUILD_TYPE}\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"optimization_level\\\":\\\"${OPTIMIZATION_LEVEL}\\\",")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_python_bindings\\\":${BUILD_PYTHON_BINDINGS},")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_tests\\\":${BUILD_TESTS},")
    file(APPEND ${BUILD_INFO_FILE} "\\\"build_experiments\\\":${BUILD_EXPERIMENTS},")
    file(APPEND ${BUILD_INFO_FILE} "\\\"enable_sanitizers\\\":${ENABLE_SANITIZERS}")
    file(APPEND ${BUILD_INFO_FILE} "}")
endfunction()

# Generate build info at configure time
execute_process(
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/cmake/generate_build_info.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# ==============================================================================
# EXPORT SUPPORT
# ==============================================================================

# Support for find_package(NLM)
include(CMakePackageConfigHelpers)

# Create NLMConfig.cmake.in template
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    "@PACKAGE_INIT@\n\n"
    "include(\\\"${CMAKE_INSTALL_LIBDIR}/cmake/NLM/NLMTargets.cmake\\\")\n\n"
    "# Add paths for components\n"
    "set(NLM_INCLUDE_DIR \"${CMAKE_INSTALL_INCLUDEDIR}\")\n"
    "set(NLM_LIBRARY_DIR \"${CMAKE_INSTALL_LIBDIR}\")\n"
    "set(NLM_BINARY_DIR \"${CMAKE_INSTALL_BINDIR}\")\n"
)

# ==============================================================================
# FINAL CONFIGURATION SUMMARY
# ==============================================================================

message(STATUS "========================================")
message(STATUS "NLM CMake Build Configuration")
message(STATUS "========================================")
message(STATUS "Project Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Optimization Level: ${OPTIMIZATION_LEVEL}")
message(STATUS "Python Bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "Test Suite: ${BUILD_TESTS}")
message(STATUS "Experiments: ${BUILD_EXPERIMENTS}")
message(STATUS "Sanitizers: ${ENABLE_SANITIZERS}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "========================================")

# ==============================================================================
# END OF CMAKE LISTS
# ==============================================================================
