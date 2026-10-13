cmake_minimum_required(VERSION 3.20)
project(NLM 
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
)

# ==============================================================================
# CMake Configuration
# ==============================================================================

# Set C++ standard
cmake_policy(SET CMP0083 NEW)  # Requires C++ standard check

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for tooling
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build")

# Install directory configuration
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix")

# ==============================================================================
# Compiler and Platform Detection
# ==============================================================================

# Feature detection for modern C++
include(CheckCXXCompilerFlag)

# Check for common compiler features
set(CompilerFlags
    -Wall
    -Wextra
    -Wpedantic
    -Werror=return-type
    -Werror=uninitialized
    -Werror=sign-compare
    -Werror=language-extension-token
    -Werror=shadow
    -Werror=formatted-security
    -Werror=extra-semi
    -Werror=old-style-cast
)

foreach(flag ${CompilerFlags})
    check_cxx_compiler_flag("${flag}" HAS_${flag})
    if(HAS_${flag})
        add_compile_options(${flag})
    endif()
endforeach()

# Platform-specific configurations
if(MSVC)
    add_compile_options(/W4 /WX)
    # Disable some warnings that are less critical for scientific code
    add_compile_options(/wd4305)  # truncation from 'double' to 'float'
    add_compile_options(/wd4244)  # conversion possible loss of data
else()
    # Add position independent code for shared libraries
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    
    # Add some platform-agnostic warnings
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wmissing-include-dirs
                          -Wnull-dereference
                          -Wduplicated-branches
                          -Wduplicated-cond
                          -Wlogical-op
                          -Wuseless-cast)
    endif()
endif()

# ==============================================================================
# Project Structure and Directories
# ==============================================================================

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(NLM_DOC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")
set(NLM_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/configs")
set(NLM_PYTHON_BINDINGS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/python")

# ==============================================================================
# Core Library Components
# ==============================================================================

# Core utilities
file(GLOB_RECURSE CORE_SOURCES
    "${NLM_SOURCE_DIR}/core/*.cpp"
    "${NLM_SOURCE_DIR}/core/*/*.cpp"
)

# Brain system
file(GLOB_RECURSE BRAIN_SOURCES
    "${NLM_SOURCE_DIR}/brain/*.cpp"
)

# Neural dynamics
file(GLOB_RECURSE DYNAMICS_SOURCES
    "${NLM_SOURCE_DIR}/dynamics/*.cpp"
)

# Plasticity
file(GLOB_RECURSE PLASTICITY_SOURCES
    "${NLM_SOURCE_DIR}/plasticity/*.cpp"
)

# Development
file(GLOB_RECURSE DEVELOPMENT_SOURCES
    "${NLM_SOURCE_DIR}/development/*.cpp"
)

# Neuromodulation
file(GLOB_RECURSE NEUROMODULATION_SOURCES
    "${NLM_SOURCE_DIR}/neuromodulation/*.cpp"
)

# Memory systems
file(GLOB_RECURSE MEMORY_SOURCES
    "${NLM_SOURCE_DIR}/memory/*.cpp"
)

# Cognition
file(GLOB_RECURSE COGNITION_SOURCES
    "${NLM_SOURCE_DIR}/cognition/*.cpp"
)

# Sensory processing
file(GLOB_RECURSE SENSORY_SOURCES
    "${NLM_SOURCE_DIR}/sensory/*.cpp"
)

# Prediction
file(GLOB_RECURSE PREDICTION_SOURCES
    "${NLM_SOURCE_DIR}/prediction/*.cpp"
)

# Motor system
file(GLOB_RECURSE MOTOR_SOURCES
    "${NLM_SOURCE_DIR}/motor/*.cpp"
)

# Environment
file(GLOB_RECURSE ENVIRONMENT_SOURCES
    "${NLM_SOURCE_DIR}/environment/*.cpp"
)

# Experiments
file(GLOB_RECURSE EXPERIMENT_SOURCES
    "${NLM_SOURCE_DIR}/experiments/*.cpp"
)

# Visualization
file(GLOB_RECURSE VISUALIZATION_SOURCES
    "${NLM_SOURCE_DIR}/visualization/*.cpp"
)

# ==============================================================================
# Main Library Target
# ==============================================================================

add_library(nlm_core STATIC
    ${CORE_SOURCES}
    ${BRAIN_SOURCES}
    ${DYNAMICS_SOURCES}
    ${PLASTICITY_SOURCES}
    ${DEVELOPMENT_SOURCES}
    ${NEUROMODULATION_SOURCES}
    ${MEMORY_SOURCES}
    ${COGNITION_SOURCES}
    ${SENSORY_SOURCES}
    ${PREDICTION_SOURCES}
    ${MOTOR_SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${EXPERIMENT_SOURCES}
    ${VISUALIZATION_SOURCES}
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:${NLM_INCLUDE_DIR}>
)

# Compiler-specific options
target_compile_features(nlm_core PUBLIC cxx_std_20)

# Position independent code
if(MSVC)
    target_compile_options(nlm_core PRIVATE /bigobj)
else()
    target_compile_options(nlm_core PRIVATE -fPIC -fstack-protector-strong)
endif()

# Source-level optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nlm_core PRIVATE -O3 -flto)
    target_link_libraries(nlm_core PRIVATE -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(nlm_core PRIVATE -O2 -g -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(nlm_core PRIVATE -O0 -g -DDEBUG)
endif()

# ==============================================================================
# Optional Components
# ==============================================================================

option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_TESTS "Build test suite" ON)
option(NLM_BUILD_EXAMPLES "Build example applications" ON)
option(NLM_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(NLM_ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(NLM_ENABLE_TSAN "Enable Thread Sanitizer" OFF)

# Python bindings configuration
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    find_package(pybind11 REQUIRED)
    
    add_library(pynlm MODULE ${NLM_PYTHON_BINDINGS_DIR}/bindings.cpp)
    target_link_libraries(pynlm PRIVATE pybind11::module nlm_core)
    
    # Python module output name
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        OUTPUT_NAME "pynlm"
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )
    
    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION "${Python3_SITEARCH}/nlm"
        RUNTIME DESTINATION "${Python3_SITEARCH}/nlm"
    )
    
    install(FILES ${NLM_PYTHON_BINDINGS_DIR}/__init__.py
        DESTINATION "${Python3_SITEARCH}/nlm"
    )
endif()

# ==============================================================================
# Additional Libraries
# ==============================================================================

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/agent>
    $<INSTALL_INTERFACE:include/agent>
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/world>
    $<INSTALL_INTERFACE:include/world>
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/experiments>
    $<INSTALL_INTERFACE:include/experiments>
)

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/experiments>
    $<INSTALL_INTERFACE:include/experiments>
)

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# ==============================================================================
# Executables
# ==============================================================================

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

# Example applications
if(NLM_BUILD_EXAMPLES)
    add_executable(nlm_example_basic
        examples/basic_example.cpp
    )
    
    target_link_libraries(nlm_example_basic PUBLIC nlm_core nlm_agent nlm_world)
    
    add_executable(nlm_example_simulation
        examples/simulation_example.cpp
    )
    
    target_link_libraries(nlm_example_simulation PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# ==============================================================================
# Testing
# ==============================================================================

if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    
    # Add test targets for easy execution
    add_test(NAME nlm_phase3_test
        COMMAND nlm_phase3_demo
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    add_test(NAME nlm_phase4_test
        COMMAND nlm_phase4_demo
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# ==============================================================================
# Installation
# ==============================================================================

# Install libraries
install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT nlmTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*/tests/*" EXCLUDE
    PATTERN "*/examples/*" EXCLUDE
)

# Install configuration files
install(DIRECTORY configs/
    DESTINATION share/nlm
    FILES_MATCHING PATTERN "*.cfg"
)

# Install documentation
install(DIRECTORY docs/
    DESTINATION share/nlm/docs
    FILES_MATCHING PATTERN "*.md"
)

# Install CMake configuration files
install(EXPORT nlmTargets
    FILE nlmConfig.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm
)

# Generate and install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nlm.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nlm.pc
    DESTINATION lib/pkgconfig
)

# ==============================================================================
# Documentation and Examples
# ==============================================================================

# Add custom targets for documentation generation
if(NLM_BUILD_DOCS)
    find_program(SPHINX_EXECUTABLE sphinx-build)
    if(SPHINX_EXECUTABLE)
        add_custom_target(docs
            COMMAND ${SPHINX_EXECUTABLE} -b html docs/ docs/_build/html
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Building documentation"
        )
    endif()
endif()

# ==============================================================================
# Testing Support (Sanitizers, Coverage)
# ==============================================================================

if(NLM_ENABLE_ASAN)
    target_compile_options(nlm_core PRIVATE -fsanitize=address)
    target_link_libraries(nlm_core PRIVATE -fsanitize=address)
endif()

if(NLM_ENABLE_TSAN)
    target_compile_options(nlm_core PRIVATE -fsanitize=thread)
    target_link_libraries(nlm_core PRIVATE -fsanitize=thread)
endif()

if(NLM_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(nlm_core PRIVATE --coverage -O0)
        target_link_libraries(nlm_core PRIVATE --coverage)
    endif()
endif()

# ==============================================================================
# Summary and Output
# ==============================================================================

message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Examples: ${NLM_BUILD_EXAMPLES}")
message(STATUS "  Address Sanitizer: ${NLM_ENABLE_ASAN}")
message(STATUS "  Thread Sanitizer: ${NLM_ENABLE_TSAN}")
message(STATUS "  Coverage: ${NLM_ENABLE_COVERAGE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
