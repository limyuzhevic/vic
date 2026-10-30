// Modern CMake configuration for pynlm with enhanced features

# Minimum CMake version for modern features
cmake_minimum_required(VERSION 3.20)

# Project definition with version and language specifications
project(pynlm_bindings
    VERSION 0.1.0-enhanced
    LANGUAGES CXX
    DESCRIPTION "Enhanced Python bindings for NLM neural simulation framework"
)

# Configure compiler flags and standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable modern CMake features
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Platform-specific configurations
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-DNOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_definitions(-D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE)
endif()

# Find Python and pybind11 dependencies
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 2.11.0 CONFIG REQUIRED)

# Configure Python module installation paths
set(PYTHON_SITE_PACKAGES "${Python_SITEARCH}")
if(PYTHON_SITE_PACKAGES STREQUAL "")
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c "import site; print(site.getsitepackages()[0])"
        OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Library configurations with modern CMake patterns
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
target_compile_features(nlm_core PUBLIC cxx_std_20)
target_compile_options(nlm_core PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Memory pool and performance optimization options
option(NLM_ENABLE_MEMORY_POOL "Enable memory pools for better performance" ON)
option(NLM_ENABLE_SIMD "Enable SIMD optimizations" ON)
option(NLM_ENABLE_PARALLEL "Enable parallel processing" OFF)

if(NLM_ENABLE_MEMORY_POOL)
    target_compile_definitions(nlm_core PRIVATE NLM_MEMORY_POOL_ENABLED)
    target_sources(nlm_core PRIVATE ${NLM_SOURCE_DIR}/performance/MemoryPool.cpp)
endif()

if(NLM_ENABLE_SIMD)
    target_compile_definitions(nlm_core PRIVATE NLM_SIMD_ENABLED)
    target_sources(nlm_core PRIVATE ${NLM_SOURCE_DIR}/performance/SIMDVectorization.cpp)
endif()

if(NLM_ENABLE_PARALLEL)
    target_compile_definitions(nlm_core PRIVATE NLM_PARALLEL_ENABLED)
    target_sources(nlm_core PRIVATE ${NLM_SOURCE_DIR}/performance/ParallelProcessing.cpp)
endif()

# Agent system library
add_library(nlm_agent STATIC)
target_sources(nlm_agent PRIVATE
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system library
add_library(nlm_world STATIC)
target_sources(nlm_world PRIVATE ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp)
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Python module with modern pybind11 features
pybind11_add_module(pynlm bindings.cpp)

# Configure module properties
target_compile_features(pynlm PRIVATE cxx_std_20)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Python module specific optimizations
if(PYBIND11_USES_MATH_COMMONDEFS)
    target_compile_definitions(pynlm PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)
endif()

# Module metadata and configuration
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX "${PYTHON_MODULE_PREFIX}"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Installation targets with modern CMake install()
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
    ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
    DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
)

# Package configuration for find_package support
include(CMakePackageConfigHelpers)

# Generate and install config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake
    @ONLY
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
)

# Documentation targets
add_custom_target(docs
    COMMAND ${Python_EXECUTABLE} -m pip install -q sphinx sphinx-rtd-theme
    COMMAND ${Python_EXECUTABLE} -m sphinx.cmd.build
        -b html
        -d _build/html/doctrees
        -q
        ${CMAKE_CURRENT_SOURCE_DIR}/docs
        ${CMAKE_CURRENT_BINARY_DIR}/docs/html
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building documentation"
    VERBATIM
)

# Testing configuration
enable_testing()

# Add Python binding tests if pytest is available
find_package(PythonInterp)
if(PythonInterp_FOUND)
    add_test(NAME python_bindings_test
        COMMAND ${Python_EXECUTABLE} -m pytest
            ${CMAKE_CURRENT_SOURCE_DIR}/python_tests
            -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Benchmark targets
add_custom_target(benchmarks
    COMMAND ${Python_EXECUTABLE} -c "import pynlm; import time; brain = pynlm.Brain(); brain.initialize(); start = time.time(); [brain.step(i) for i in range(1000)]; end = time.time(); print(f'Steps/sec: {1000/(end-start):.2f}')"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Run quick performance benchmark"
    VERBATIM
)

# Development convenience targets
add_custom_target(develop
    COMMAND echo "Available targets:"
    COMMAND echo "  build - Build the project"
    COMMAND echo "  test - Run tests"
    COMMAND echo "  docs - Build documentation"
    COMMAND echo "  benchmark - Run benchmarks"
    COMMENT "Show available development targets"
    VERBATIM
)

# Summary
message(STATUS "Configuring NLM Python bindings with modern CMake")
message(STATUS "  Python executable: ${Python_EXECUTABLE}")
message(STATUS "  Python version: ${Python_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Memory pools: ${NLM_ENABLE_MEMORY_POOL}")
message(STATUS "  SIMD optimizations: ${NLM_ENABLE_SIMD}")
message(STATUS "  Parallel processing: ${NLM_ENABLE_PARALLEL}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
