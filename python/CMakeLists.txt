cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX
    DESCRIPTION "Python bindings for NLM (Neural Learning Machine)"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Position-independent code required for Python modules
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find Python and pybind11 with proper dependency checking
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set source directory
local(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" CACHE INTERNAL "NLM source directory")

# Compiler warning configuration
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -fPIC)
endif()

# Modern library organization with modular design

# Core library with configuration management
add_library(nlm_core STATIC)
target_sources(nlm_core PRIVATE
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror -fPIC>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)
set_target_properties(nlm_core PROPERTIES
    CXX_STANDARD 20
    POSITION_INDEPENDENT_CODE TRUE
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)

# Agent system (provides AgentBrain and SensoryPercept interfaces)
add_library(nlm_agent STATIC)
target_sources(nlm_agent PRIVATE
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)
set_target_properties(nlm_agent PROPERTIES CXX_STANDARD 20)

# World system (provides SimpleWorld environment)
add_library(nlm_world STATIC)
target_sources(nlm_world PRIVATE ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp)
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)
set_target_properties(nlm_world PROPERTIES CXX_STANDARD 20)

# Phase 3 library (world interaction experiments)
add_library(nlm_phase3 STATIC)
target_sources(nlm_phase3 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp)
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
set_target_properties(nlm_phase3 PROPERTIES CXX_STANDARD 20)

# Phase 4 library (cognitive function experiments)
add_library(nlm_phase4 STATIC)
target_sources(nlm_phase4 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp)
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
set_target_properties(nlm_phase4 PROPERTIES CXX_STANDARD 20)

# Phase 5 library (performance optimization experiments)
add_library(nlm_phase5 STATIC)
target_sources(nlm_phase5 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase5Experiment.cpp)
target_include_directories(nlm_phase5 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
set_target_properties(nlm_phase5 PROPERTIES CXX_STANDARD 20)

# Phase 6 library (integration experiments)
add_library(nlm_phase6 STATIC)
target_sources(nlm_phase6 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp)
target_include_directories(nlm_phase6 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)
set_target_properties(nlm_phase6 PROPERTIES CXX_STANDARD 20)

# Modern Python module definition using pybind11
pybind11_add_module(pynlm ${CMAKE_CURRENT_SOURCE_DIR}/bindings.cpp)

# Target-specific Python module properties
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Python-specific compiler options for the module
target_compile_options(pynlm PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -fPIC>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# MSVC-specific options
target_compile_options(pynlm PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->
)

# Module metadata and configuration
set_target_properties(pynlm PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    OUTPUT_NAME "pynlm"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python_modules"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python_modules"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Installation configuration for Python package
include(CMakePackageConfigHelpers)

# Configure Python module installation
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_SITEARCH}/pynlm
    ARCHIVE DESTINATION ${PYTHON_SITEARCH}/pynlm
    RUNTIME DESTINATION ${PYTHON_SITEARCH}/pynlm
)

# Install Python module headers and documentation
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../easy_usage.md
    DESTINATION ${PYTHON_SITEARCH}/pynlm
    RENAME README.md
)

# Add custom targets for Python development
add_custom_target(pynlm_build
    COMMENT "Building NLM Python module with scikit-build-core compatible configuration"
    VERBATIM
)

add_custom_target(pynlm_install
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${PYTHON_PREFIX}
    COMMENT "Installing NLM Python module to Python site-packages"
    VERBATIM
)

# Generate Python package metadata
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake
    @ONLY
)

# Create and install Python package configuration
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake
    DESTINATION ${PYTHON_SITEARCH}/pynlm/cmake
)

# Summary information for users
message(STATUS "Python Module Configuration:")
message(STATUS "  Module name: pynlm")
message(STATUS "  Python version: ${PYTHON_VERSION}")
message(STATUS "  Module extension: ${PYTHON_MODULE_EXTENSION}")
message(STATUS "  Installation prefix: ${PYTHON_PREFIX}")
message(STATUS "  Site-packages directory: ${PYTHON_SITEARCH}")
