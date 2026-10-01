cmake_minimum_required(VERSION 3.16)

# Project configuration
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
)

# ==============================================================================
# MODERN CMAKE PRACTICES
# ==============================================================================

# Set C++ standard with modern approach
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Policy settings for modern CMake
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)  # Use <Package>_ROOT variables
endif()

# Import CMake functions and modules
include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ==============================================================================
# BUILD OPTIONS AND FEATURE FLAGS
# ==============================================================================

# Core components
option(NLM_BUILD_CORE "Build core utilities" ON)
option(NLM_BUILD_BRAIN "Build neural brain components" ON)
option(NLM_BUILD_COGNITION "Build cognitive systems" ON)
option(NLM_BUILD_MEMORY "Build memory systems" ON)
option(NLM_BUILD_SENSORY "Build sensory systems" ON)
option(NLM_BUILD_MOTOR "Build motor systems" ON)
option(NLM_BUILD_VISUALIZATION "Build visualization components" ON)
option(NLM_BUILD_AGENT "Build agent systems" ON)
option(NLM_BUILD_WORLD "Build world simulation systems" ON)
option(NLM_BUILD_EXPERIMENTS "Build experiment systems" ON)

# Python bindings
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_PYTHON_EXECUTABLE "Path to Python executable" "")
option(NLM_PYTHON_VERSION "Required Python version" "3.8")

# Testing
option(NLM_BUILD_TESTS "Build test suite" ON)
option(NLM_BUILD_BENCHMARKS "Build performance benchmarks" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable address/memory sanitizers" OFF)
option(NLM_ENABLE_PROFILING "Enable profiling support" OFF)
option(NLM_ENABLE_ASSERTIONS "Enable debug assertions" OFF)

# Optimization levels
option(NLM_OPTIMIZATION_LEVEL "Optimization level (0, 1, 2, 3)" 2)

# ==============================================================================
# PLATFORM-SPECIFIC CONFIGURATION
# ==============================================================================

# Generator expressions for platform-specific settings
if(MSVC)
    add_compile_options(/permissive-)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Platform-specific definitions using generator expressions
add_compile_definitions(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-DNLM_PLATFORM_UNIX>
    $<$<CXX_COMPILER_ID:MSVC>:-DNLM_PLATFORM_WINDOWS>
    $<$<CONFIG:Debug>:-DNLM_DEBUG>
    $<$<BOOL:${NLM_ENABLE_ASSERTIONS}>:-DNLM_ENABLE_ASSERTIONS>
)

# ==============================================================================
# DEPENDENCY MANAGEMENT
# ==============================================================================

# Configure Python
if(NLM_BUILD_PYTHON_BINDINGS)
    if(NOT NLM_PYTHON_EXECUTABLE)
        find_package(Python ${NLM_PYTHON_VERSION} REQUIRED COMPONENTS Interpreter Development)
    else()
        find_package(Python ${NLM_PYTHON_VERSION} REQUIRED COMPONENTS Interpreter Development)
        set(Python_EXECUTABLE ${NLM_PYTHON_EXECUTABLE})
        set(Python_FOUND TRUE)
    endif()
endif()

# Find or download pybind11 using FetchContent
if(NLM_BUILD_PYTHON_BINDINGS)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.0
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# ==============================================================================
# COMPONENT GROUP DEFINITIONS
# ==============================================================================

# Create interface libraries for component groups
if(NLM_BUILD_CORE)
    add_library(nlm_core_interface INTERFACE)
    target_include_directories(nlm_core_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(nlm_core_interface INTERFACE cxx_std_20)
    target_compile_options(nlm_core_interface INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    target_link_libraries(nlm_core_interface INTERFACE nlm_core)
endif()

if(NLM_BUILD_BRAIN)
    add_library(nlm_brain_interface INTERFACE)
    target_include_directories(nlm_brain_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/brain>
    )
    target_compile_features(nlm_brain_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_brain_interface INTERFACE nlm_core nlm_brain)
endif()

if(NLM_BUILD_COGNITION)
    add_library(nlm_cognition_interface INTERFACE)
    target_include_directories(nlm_cognition_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cognition>
    )
    target_compile_features(nlm_cognition_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_cognition_interface INTERFACE nlm_core nlm_cognition)
endif()

if(NLM_BUILD_MEMORY)
    add_library(nlm_memory_interface INTERFACE)
    target_include_directories(nlm_memory_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/memory>
    )
    target_compile_features(nlm_memory_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_memory_interface INTERFACE nlm_core nlm_memory)
endif()

if(NLM_BUILD_SENSORY)
    add_library(nlm_sensory_interface INTERFACE)
    target_include_directories(nlm_sensory_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/sensory>
    )
    target_compile_features(nlm_sensory_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_sensory_interface INTERFACE nlm_core nlm_sensory)
endif()

if(NLM_BUILD_MOTOR)
    add_library(nlm_motor_interface INTERFACE)
    target_include_directories(nlm_motor_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/motor>
    )
    target_compile_features(nlm_motor_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_motor_interface INTERFACE nlm_core nlm_motor)
endif()

if(NLM_BUILD_VISUALIZATION)
    add_library(nlm_visualization_interface INTERFACE)
    target_include_directories(nlm_visualization_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/visualization>
    )
    target_compile_features(nlm_visualization_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_visualization_interface INTERFACE nlm_core nlm_visualization)
endif()

if(NLM_BUILD_AGENT)
    add_library(nlm_agent_interface INTERFACE)
    target_include_directories(nlm_agent_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/agent>
    )
    target_compile_features(nlm_agent_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_agent_interface INTERFACE nlm_core nlm_agent)
endif()

if(NLM_BUILD_WORLD)
    add_library(nlm_world_interface INTERFACE)
    target_include_directories(nlm_world_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/world>
    )
    target_compile_features(nlm_world_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_world_interface INTERFACE nlm_core nlm_world)
endif()

if(NLM_BUILD_EXPERIMENTS)
    add_library(nlm_experiments_interface INTERFACE)
    target_include_directories(nlm_experiments_interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/experiments>
    )
    target_compile_features(nlm_experiments_interface INTERFACE cxx_std_20)
    target_link_libraries(nlm_experiments_interface INTERFACE nlm_core nlm_experiments)
endif()

# ==============================================================================
# COMPONENT IMPLEMENTATION CREATION
# ==============================================================================

# Core component
if(NLM_BUILD_CORE)
    add_library(nlm_core STATIC)
    
    # Core source files
    target_sources(nlm_core PRIVATE
        src/core/Types/Types.cpp
        src/core/Config/Config.cpp
        src/core/Random/Random.cpp
        src/core/Logger/Logger.cpp
        src/core/SimulationClock/SimulationClock.cpp
    )
    
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core>
        $<INSTALL_INTERFACE:include>
    )
    
    target_compile_features(nlm_core PUBLIC cxx_std_20)
    
    # Platform-specific optimizations
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-O${NLM_OPTIMIZATION_LEVEL}>
        $<$<CXX_COMPILER_ID:MSVC>:/O${NLM_OPTIMIZATION_LEVEL}>
    )
    
    if(NLM_ENABLE_SANITIZERS)
        target_compile_options(nlm_core PRIVATE -fsanitize=address,undefined)
        target_link_options(nlm_core PRIVATE -fsanitize=address,undefined)
    endif()
endif()

# Brain component
if(NLM_BUILD_BRAIN)
    add_library(nlm_brain STATIC)
    
    target_sources(nlm_brain PRIVATE
        src/brain/Neuron.cpp
        src/brain/Synapse.cpp
        src/brain/NeuralPopulation.cpp
        src/brain/NeuralRegion.cpp
        src/brain/Brain.cpp
    )
    
    target_include_directories(nlm_brain PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/brain>
    )
    
    target_compile_features(nlm_brain PUBLIC cxx_std_20)
    target_link_libraries(nlm_brain PUBLIC nlm_core)
endif()

# Cognition component
if(NLM_BUILD_COGNITION)
    add_library(nlm_cognition STATIC)
    
    target_sources(nlm_cognition PRIVATE
        src/cognition/ConceptFormation.cpp
        src/cognition/NeuralPlanner.cpp
    )
    
    target_include_directories(nlm_cognition PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/cognition>
    )
    
    target_compile_features(nlm_cognition PUBLIC cxx_std_20)
    target_link_libraries(nlm_cognition PUBLIC nlm_core nlm_brain)
endif()

# Memory component
if(NLM_BUILD_MEMORY)
    add_library(nlm_memory STATIC)
    
    target_sources(nlm_memory PRIVATE
        src/memory/WorkingMemory.cpp
        src/memory/EpisodicMemory.cpp
        src/memory/SemanticMemory.cpp
        src/memory/ProceduralMemory.cpp
        src/memory/AssociativeMemory.cpp
        src/memory/NeuralWorkingMemory.cpp
        src/memory/NeuralEpisodicMemory.cpp
    )
    
    target_include_directories(nlm_memory PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/memory>
    )
    
    target_compile_features(nlm_memory PUBLIC cxx_std_20)
    target_link_libraries(nlm_memory PUBLIC nlm_core nlm_brain)
endif()

# Sensory component
if(NLM_BUILD_SENSORY)
    add_library(nlm_sensory STATIC)
    
    target_sources(nlm_sensory PRIVATE
        src/sensory/SensoryInput.cpp
        src/sensory/Vision.cpp
        src/sensory/Audio.cpp
        src/sensory/InternalSignals.cpp
    )
    
    target_include_directories(nlm_sensory PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/sensory>
    )
    
    target_compile_features(nlm_sensory PUBLIC cxx_std_20)
    target_link_libraries(nlm_sensory PUBLIC nlm_core)
endif()

# Motor component
if(NLM_BUILD_MOTOR)
    add_library(nlm_motor STATIC)
    
    target_sources(nlm_motor PRIVATE
        src/motor/MotorSystem.cpp
    )
    
    target_include_directories(nlm_motor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/motor>
    )
    
    target_compile_features(nlm_motor PUBLIC cxx_std_20)
    target_link_libraries(nlm_motor PUBLIC nlm_core)
endif()

# Visualization component
if(NLM_BUILD_VISUALIZATION)
    add_library(nlm_visualization STATIC)
    
    target_sources(nlm_visualization PRIVATE
        src/visualization/VisualizationInterface.cpp
    )
    
    target_include_directories(nlm_visualization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/visualization>
    )
    
    target_compile_features(nlm_visualization PUBLIC cxx_std_20)
    target_link_libraries(nlm_visualization PUBLIC nlm_core)
endif()

# Agent component
if(NLM_BUILD_AGENT)
    add_library(nlm_agent STATIC)
    
    target_sources(nlm_agent PRIVATE
        src/agent/SensoryPercept.cpp
        src/agent/AgentBrain.cpp
    )
    
    target_include_directories(nlm_agent PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/agent>
    )
    
    target_compile_features(nlm_agent PUBLIC cxx_std_20)
    target_link_libraries(nlm_agent PUBLIC nlm_core nlm_brain nlm_cognition nlm_memory)
endif()

# World component
if(NLM_BUILD_WORLD)
    add_library(nlm_world STATIC)
    
    target_sources(nlm_world PRIVATE
        src/world/SimpleWorld.cpp
    )
    
    target_include_directories(nlm_world PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/world>
    )
    
    target_compile_features(nlm_world PUBLIC cxx_std_20)
    target_link_libraries(nlm_world PUBLIC nlm_core)
endif()

# Experiments component
if(NLM_BUILD_EXPERIMENTS)
    add_library(nlm_experiments STATIC)
    
    target_sources(nlm_experiments PRIVATE
        src/experiments/Experiment.cpp
        src/experiments/ExperimentRunner.cpp
        src/experiments/Metrics.cpp
    )
    
    target_include_directories(nlm_experiments PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/experiments>
    )
    
    target_compile_features(nlm_experiments PUBLIC cxx_std_20)
    target_link_libraries(nlm_experiments PUBLIC nlm_core nlm_agent nlm_world)
endif()

# ==============================================================================
# EXECUTABLES
# ==============================================================================

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# Phase 6 demo executable
add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_experiments)

# ==============================================================================
# PYTHON BINDINGS
# ==============================================================================

if(NLM_BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# ==============================================================================
# TESTING
# ==============================================================================

if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(NLM_BUILD_BENCHMARKS)
    add_executable(nlm_benchmark src/benchmark.cpp)
    target_link_libraries(nlm_benchmark PUBLIC nlm_core nlm_brain nlm_agent nlm_world nlm_experiments)
    
    if(NLM_ENABLE_PROFILING)
        target_compile_definitions(nlm_benchmark PRIVATE NLM_ENABLE_PROFILING)
    endif()
endif()

# ==============================================================================
# COVERAGE AND ANALYSIS
# ==============================================================================

if(NLM_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_core PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(nlm_core PRIVATE --coverage)
        
        add_custom_target(coverage_report
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND lcov --directory . --capture --output-file coverage.info
            COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND genhtml -o coverage_html coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report"
        )
    endif()
endif()

# ==============================================================================
# INSTALLATION
# ==============================================================================

# Configure package for installation
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install targets
install(TARGETS nlm nlm_core nlm_brain nlm_agent nlm_world nlm_experiments
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install Python bindings
if(NLM_BUILD_PYTHON_BINDINGS)
    install(TARGETS pynlm
        LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
        ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}
        RUNTIME DESTINATION ${PYTHON_SITE_PACKAGES}
    )
endif()

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Install CMake config
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Install export configuration
install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# ==============================================================================
# DOCUMENTATION
# ==============================================================================

# Doxygen configuration
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/ doxygen/html)
        add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()
