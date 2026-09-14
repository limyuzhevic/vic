cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Neuromorphic Learning Machine")

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type configuration
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Default compiler flags by configuration
add_compile_options(
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
    $<$<CONFIG:MinSizeRel>:-Os>
)

# Platform-specific settings
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_definitions(-D_GNU_SOURCE)
endif()

# Source and build directories
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(NLM_MODULES_DIR "${NLM_SOURCE_DIR}")

# Dependency management
include(FetchContent)
include(CMakeFindDependencyMacro)

# Configuration options
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_EXAMPLES "Build example applications" ON)
option(NLM_ENABLE_VALGRIND "Enable valgrind memory debugging" OFF)
option(NLM_BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(NLM_BUILD_EXPERIMENTS "Build experimental features" ON)
option(NLM_ENABLE_PERFORMANCE_FEATURES "Enable performance optimizations" ON)
option(NLM_ENABLE_ACCELERATION "Enable hardware acceleration" OFF)

if(NLM_BUILD_SHARED_LIBS)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Dependency Categories
add_subdirectory(src/core)
add_subdirectory(src/brain)
add_subdirectory(src/dynamics)
add_subdirectory(src/plasticity)
add_subdirectory(src/development)
add_subdirectory(src/neuromodulation)
add_subdirectory(src/memory)
add_subdirectory(src/cognition)
add_subdirectory(src/sensory)
add_subdirectory(src/prediction)
add_subdirectory(src/motor)
add_subdirectory(src/environment)
add_subdirectory(src/experiments)
add_subdirectory(src/visualization)
add_subdirectory(src/agent)
add_subdirectory(src/world)
add_subdirectory(src/experiments)

# Create interface libraries for component aggregation
set(NLM_LIBRARIES nlm_core nlm_agent nlm_world)

if(TARGET nlm_phase3)
    list(APPEND NLM_LIBRARIES nlm_phase3)
endif()

if(TARGET nlm_phase4)
    list(APPEND NLM_LIBRARIES nlm_phase4)
endif()

# Performance optimization configuration
if(NLM_ENABLE_PERFORMANCE_FEATURES)
    target_compile_options(nlm_core PRIVATE -march=native -ffast-math)
endif()

if(NLM_ENABLE_ACCELERATION)
    find_package(OpenMP REQUIRED)
    target_link_libraries(nlm_core PUBLIC OpenMP::OpenMP_CXX)
endif()

# Main library targets
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

# Core dependencies (Headers only)
add_library(nlm_core_headers INTERFACE)
target_include_directories(nlm_core_headers
    INTERFACE
    $<BUILD_INTERFACE:${NLM_MODULES_DIR}/core>
    $<INSTALL_INTERFACE:include>
)

# Brain module
set(NLM_BRAIN_SOURCES
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
)

add_library(nlm_brain STATIC ${NLM_BRAIN_SOURCES})
target_include_directories(nlm_brain PUBLIC ${NLM_MODULES_DIR}/brain)
target_link_libraries(nlm_brain PUBLIC nlm_core)

# Core component targets
set(NLM_DYNAMICS_SOURCES ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp)
add_library(nlm_dynamics STATIC ${NLM_DYNAMICS_SOURCES})

# Dynamically loadable modules for better extensibility
set(NLM_PLATFORMS_LIBS ${NLM_LIBRARIES})

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

# Phase 6 demo executable
if(NLM_BUILD_EXPERIMENTS)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Testing
enable_testing()
if(NLM_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Python bindings
if(NLM_BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(NLM_BUILD_PYTHON_BINDINGS)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python/
        DESTINATION python
        FILES_MATCHING PATTERN "*.cpp" PATTERN "CMakeLists.txt"
    )
endif()

# Documentation targets
if(NLM_BUILD_EXAMPLES)
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Building documentation..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Package configuration for find_package support
include(GNUInstallDirs)

# Create NLMConfig.cmake for package management
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Summary
message(STATUS "NLM Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Shared libraries: ${NLM_BUILD_SHARED_LIBS}")
message(STATUS "  Performance features: ${NLM_ENABLE_PERFORMANCE_FEATURES}")
message(STATUS "  Hardware acceleration: ${NLM_ENABLE_ACCELERATION}")
message(STATUS "  Core libraries: nlm_core, nlm_agent, nlm_world")
if(TARGET nlm_phase3)
    message(STATUS "  Phase 3: Enabled")
endif()
if(TARGET nlm_phase4)
    message(STATUS "  Phase 4: Enabled")
endif()
if(TARGET nlm_phase6_demo)
    message(STATUS "  Phase 6: Enabled")
endif()

# Configure file generation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    @ONLY
)
