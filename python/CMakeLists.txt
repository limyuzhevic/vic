cmake_minimum_required(VERSION 3.19)
project(pynlm_bindings LANGUAGES CXX)

# Standardized build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build types" FORCE)

# C++20 standard with required features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Comprehensive warning levels function
function(setup_pybind_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target_name} PRIVATE -Wshadow -Wnon-virtual-dtor -Wold-style-cast)
        endif()
    endif()
endfunction()

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Standardized source directory using relative paths
# This is the NLM source directory relative to the python/CMakeLists.txt location
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Platform-specific optimizations for Python bindings
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NLM_ENABLE_PERFORMANCE)
    add_compile_options(-march=native -flto)
    add_link_options(-flto)
elseif(NLM_ENABLE_PERFORMANCE)
    add_compile_options(-O3)
endif()

# Core library components with standardized paths
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    # Additional core components as needed
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
)

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
setup_pybind_warnings(nlm_core)

# Agent library with standardized paths
set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)
setup_pybind_warnings(nlm_agent)

# World library with standardized paths
set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)
setup_pybind_warnings(nlm_world)

# Python module creation with pybind11
pybind11_add_module(pynlm bindings.cpp)

# Target link libraries with modern CMake
set(PYNLM_LINK_LIBRARIES nlm_core nlm_agent nlm_world)
target_link_libraries(pynlm PRIVATE ${PYNLM_LINK_LIBRARIES})

# Standardized include directories
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Compiler-specific configurations
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive- /bigobj)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pynlm PRIVATE -fPIC -fstack-protector-strong)
endif()

# Sanitizer configurations for development
if(NLM_ENABLE_ASAN AND NOT MSVC)
    target_compile_options(pynlm PRIVATE -fsanitize=address)
    target_link_options(pynlm PRIVATE -fsanitize=address)
endif()

if(NLM_ENABLE_TSAN AND NOT MSVC)
    target_compile_options(pynlm PRIVATE -fsanitize=thread)
    target_link_options(pynlm PRIVATE -fsanitize=thread)
endif()

# Install Python module with comprehensive targets
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}/pynlm
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install Python package structure
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${Python_SITEARCH}/pynlm
    PATTERN "*.cpp"
    PATTERN "*.hpp"
    PATTERN "*.h"
    PATTERN "CMakeLists.txt"
    PATTERN "*.py"
    PATTERN "*.pyi"
    PATTERN "*.txt"
)

# Install package metadata for Python distribution
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml"
    COPYONLY
)

# Create a setup.py for pip installation
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/setup.py
"import setuptools
import sys
import os

# Get Python version
python_version = sys.version_info
if python_version.major == 3 and python_version.minor >= 8:
    python_requires = '>=3.8'
else:
    python_requires = '>=3.6'

# Read requirements from requirements.txt
with open('nlm_utilities/requirements.txt', 'r') as f:
    requirements = f.read().splitlines()

setup(
    name='pynlm',
    version='0.1.0',
    description='Python bindings for NLM neural simulation framework',
    long_description='NLM (Neural Learning Machine) Python Bindings provide Python interfaces to the NLM C++ neural simulation framework.',
    author='NLM Developers',
    author_email='contact@nlm.org',
    url='https://github.com/nlm-project/nlm',
    packages=setuptools.find_packages(),
    package_data={
        '': ['*.so', '*.pyd', '*.txt', '*.json'],
        'nlm_utilities': ['*.json', '*.yaml', '*.yml'],
    },
    include_package_data=True,
    install_requires=requirements,
    python_requires=python_requires,
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
    ],
    zip_safe=False,
)
")

# Install the setup.py file
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/setup.py
    DESTINATION ${Python_SITEARCH}/pynlm
)

# Create a Python package marker file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pynlm/__init__.py
"\"\"\"NLM Python Bindings Package

This package provides Python interfaces to the NLM (Neural Learning Machine)
neural simulation framework.

The NLM is an experimental artificial developmental brain project that
creates neural systems learning from experience through interaction with
environments.
\"\"\"

__version__ = '0.1.0'
")

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm/__init__.py
    DESTINATION ${Python_SITEARCH}/pynlm/pynlm
)

# Provide export targets for find_package support
export(TARGETS pynlm nlm_core nlm_agent nlm_world
    FILE "${CMAKE_CURRENT_BINARY_DIR}/pynlmTargets.cmake"
)

install(EXPORT pynlmTargets
    FILE pynlmTargets.cmake
    DESTINATION ${Python_SITEARCH}/pynlm/cmake
)

# Generate and install a config file
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfigVersion.cmake"
    VERSION 0.1.0
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pynlmConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake"
    INSTALL_DESTINATION ${Python_SITEARCH}/pynlm/cmake
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/pynlmConfigVersion.cmake"
    DESTINATION ${Python_SITEARCH}/pynlm/cmake
)