# Enhanced Python bindings CMake configuration
# Integrates with main NLM CMake system

cmake_minimum_required(VERSION 3.16)

# This should be a separate project that links with the main NLM library
project(pynlm_bindings LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use the same build type as the main project
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Import the main NLM library configuration
include(${CMAKE_CURRENT_LIST_DIR}/cmake/NLMConfig.cmake REQUIRED)

# Define source files for Python bindings
set(NLM_CORE_BINDINGS_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

set(NLM_AGENT_BINDINGS_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

set(NLM_WORLD_BINDINGS_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# Import NLM library targets
add_library(nlm_core_bindings STATIC ${NLM_CORE_BINDINGS_SOURCES})
target_include_directories(nlm_core_bindings PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core_bindings PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_library(nlm_agent_bindings STATIC ${NLM_AGENT_BINDINGS_SOURCES})
target_include_directories(nlm_agent_bindings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent_bindings PUBLIC nlm_core_bindings)

add_library(nlm_world_bindings STATIC ${NLM_WORLD_BINDINGS_SOURCES})
target_include_directories(nlm_world_bindings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world_bindings PUBLIC nlm_core_bindings)

# Create Python module using pybind11
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core_bindings nlm_agent_bindings nlm_world_bindings)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Platform-specific compiler options
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
    target_link_options(pynlm PRIVATE /WX)
else()
    target_compile_options(pynlm PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Apply common platform flags
target_compile_features(pynlm PRIVATE cxx_std_20)

# Installation configuration
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}/nlm
    ARCHIVE DESTINATION ${Python_SITEARCH}/nlm
    RUNTIME DESTINATION ${Python_SITEARCH}/nlm
)

# Install headers if needed
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Types/Types.hpp
              ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Config.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm/core
)
