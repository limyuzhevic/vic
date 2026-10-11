# Python Bindings CMake Configuration
# Enhanced for comprehensive testing and validation

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Enhanced source directory with all NLM components
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Core system components
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Config/ConfigEnhanced.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

# Brain components
function (add_brain_sources var)
    foreach(file ${ARGN})
        list(APPEND ${var} "${NLM_SOURCE_DIR}/brain/${file}.cpp")
    endforeach()
    set(${var} ${${var}} PARENT_SCOPE)
endfunction()

add_brain_sources(NLM_BRAIN_SOURCES
    Neuron
    Synapse
    NeuralPopulation
    NeuralRegion
    Brain
)

# System components
function (add_system_sources var)
    foreach(file ${ARGN})
        list(APPEND ${var} "${NLM_SOURCE_DIR}/${file}.cpp")
    endforeach()
    set(${var} ${${var}} PARENT_SCOPE)
endfunction()

add_system_sources(NLM_DYNAMICS_SOURCES NeuralDynamics SpikeSystem)
add_system_sources(NLM_PLASTICITY_SOURCES PlasticityRule Hebbian STDP StructuralPlasticity)
add_system_sources(NLM_DEVELOPMENT_SOURCES DevelopmentSystem Synaptogenesis Pruning Maturation)
add_system_sources(NLM_NEUROMODULATION_SOURCES Neuromodulator Reward PredictionError Novelty Curiosity)
add_system_sources(NLM_MEMORY_SOURCES WorkingMemory EpisodicMemory SemanticMemory ProceduralMemory AssociativeMemory NeuralWorkingMemory NeuralEpisodicMemory)
add_system_sources(NLM_COGNITION_SOURCES ConceptFormation NeuralPlanner)
add_system_sources(NLM_SENSORY_SOURCES SensoryInput Vision Audio InternalSignals)
add_system_sources(NLM_PREDICTION_SOURCES PredictionSystem NeuralPrediction)
add_system_sources(NLM_MOTOR_SOURCES MotorSystem)
add_system_sources(NLM_ENVIRONMENT_SOURCES Environment Observation Action)
add_system_sources(NLM_EXPERIMENTS_SOURCES Experiment ExperimentRunner Metrics)
add_system_sources(NLM_VISUALIZATION_SOURCES VisualizationInterface)

# Agent components
add_system_sources(NLM_AGENT_SOURCES SensoryPercept AgentBrain)

# World components
add_system_sources(NLM_WORLD_SOURCES SimpleWorld)

# Configure enhanced Python module
pybind11_add_module(pynlm_enhanced python/bindings.cpp)

target_include_directories(pynlm_enhanced PRIVATE ${NLM_SOURCE_DIR})

target_sources(pynlm_enhanced PRIVATE
    ${NLM_CORE_SOURCES}
    ${NLM_BRAIN_SOURCES}
    ${NLM_DYNAMICS_SOURCES}
    ${NLM_PLASTICITY_SOURCES}
    ${NLM_DEVELOPMENT_SOURCES}
    ${NLM_NEUROMODULATION_SOURCES}
    ${NLM_MEMORY_SOURCES}
    ${NLM_COGNITION_SOURCES}
    ${NLM_SENSORY_SOURCES}
    ${NLM_PREDICTION_SOURCES}
    ${NLM_MOTOR_SOURCES}
    ${NLM_ENVIRONMENT_SOURCES}
    ${NLM_EXPERIMENTS_SOURCES}
    ${NLM_VISUALIZATION_SOURCES}
    ${NLM_AGENT_SOURCES}
    ${NLM_WORLD_SOURCES}
)

target_link_libraries(pynlm_enhanced PRIVATE nlm_core nlm_agent nlm_world nlm_config_enhanced)

if(MSVC)
    target_compile_options(pynlm_enhanced PRIVATE /permissive-)
endif()

# Legacy module for backward compatibility
pybind11_add_module(pynlm python/bindings.cpp)

target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Install enhanced Python module
install(TARGETS pynlm_enhanced
    LIBRARY DESTINATION ${Python_SITEARCH}
    ARCHIVE DESTINATION ${Python_SITEARCH}
    RUNTIME DESTINATION ${Python_SITEARCH}
)

# Install legacy Python module
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}
    ARCHIVE DESTINATION ${Python_SITEARCH}
    RUNTIME DESTINATION ${Python_SITEARCH}
)

# Create Python package structure
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pynlm)

# Create __init__.py that imports from enhanced bindings
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/pynlm/__init__.py
    "from .pynlm_enhanced import *\n"
    "__version__ = \"2.0.0-enhanced\"\n"
    "__description__ = \"Enhanced NLM Python bindings with validation and advanced features\"\n"
)

# Create setup.py for enhanced Python package
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/setup.py
    "from setuptools import setup, Extension\n"
    "from setuptools.command.build_ext import build_ext\n"
    "import sys\n"
    "\n"
    "class Pybind11Extension(Extension):\n"
    "    def __init__(self, name, sources):\n"
    "        Extension.__init__(self, name, sources, extra_compile_args=['-std=c++20', '-Wall', '-Wextra', '-Wpedantic'] if sys.platform != 'win32' else ['/W4'], language='c++')\n"
    "\n"
    "setup(\n"
    "    name='pynlm',\n"
    "    version='2.0.0-enhanced',\n"
    "    description='Enhanced NLM (Neural Learning Machine) Python bindings',\n"
    "    long_description='Enhanced Python bindings for the NLM C++ neural simulation framework with validation, context managers, and advanced features.',\n"
    "    author='NLM Authors',\n"
    "    author_email='nlm@example.com',\n"
    "    url='https://github.com/nlm-project/nlm',\n"
    "    packages=['pynlm'],\n"
    "    ext_modules=[Pybind11Extension('pynlm.pynlm_enhanced', ['python/bindings.cpp'])],\n"
    "    install_requires=[],\n"
    "    classifiers=[\n"
    "        'Development Status :: 3 - Alpha',\n"
    "        'Intended Audience :: Science/Research',\n"
    "        'License :: OSI Approved :: MIT License',\n"
    "        'Programming Language :: Python :: 3',\n"
    "        'Programming Language :: C++',\n"
    "        'Topic :: Scientific/Engineering :: Artificial Intelligence',\n"
    "    ],\n"
    "    python_requires='>=3.8',\n"
    ")\n"
)
