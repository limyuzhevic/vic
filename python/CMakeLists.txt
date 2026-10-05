# NLM Python Bindings - Modern CMake Configuration
cmake_minimum_required(VERSION 3.22)

project(pynlm_bindings
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Python bindings for NLM neural simulation framework"
)

# Python and pybind11 detection
set(PYTHON_BINDING_REQUIRED ON CACHE BOOL "Build Python bindings")

if(NOT PYTHON_BINDING_REQUIRED)
    return()
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED CONFIG)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler configuration
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

# Python include directories
set(PYTHON_INCLUDE_DIRS
    ${Python_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIR}
)

# NLM core library source files
set(NLM_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Types/Types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Random/Random.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Logger/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/SimulationClock/SimulationClock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain/Neuron.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain/Synapse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain/NeuralPopulation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain/NeuralRegion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain/Brain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dynamics/NeuralDynamics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/dynamics/SpikeSystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity/PlasticityRule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity/Hebbian.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity/STDP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity/StructuralPlasticity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/development/DevelopmentSystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/development/Synaptogenesis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/development/Pruning.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/development/Maturation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation/Neuromodulator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation/Reward.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation/PredictionError.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation/Novelty.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/neuromodulation/Curiosity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/WorkingMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/EpisodicMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/SemanticMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/ProceduralMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/AssociativeMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/NeuralWorkingMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/memory/NeuralEpisodicMemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition/ConceptFormation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/cognition/NeuralPlanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory/SensoryInput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory/Vision.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory/Audio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/sensory/InternalSignals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/prediction/PredictionSystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/prediction/NeuralPrediction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/motor/MotorSystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/environment/Environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/environment/Observation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/environment/Action.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments/Experiment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments/ExperimentRunner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments/Metrics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/visualization/VisualizationInterface.cpp
)

# Agent library sources
set(NLM_AGENT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/agent/SensoryPercept.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/agent/AgentBrain.cpp
)

# World library sources
set(NLM_WORLD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/world/SimpleWorld.cpp
)

# Build core components as static libraries
add_library(nlm_core_py STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core_py PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(nlm_core_py PUBLIC cxx_std_20)

add_library(nlm_agent_py STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent_py PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent_py PUBLIC nlm_core_py)

add_library(nlm_world_py STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world_py PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world_py PUBLIC nlm_core_py)

# Python module using pybind11
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core_py nlm_agent_py nlm_world_py)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Platform-specific compiler options
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
    target_compile_definitions(pynlm PRIVATE _HAS_EXCEPTIONS=1 _HAS_RTTI=1)
else()
    target_compile_options(pynlm PRIVATE -Wall -Wextra -Wpedantic -Werror=return-type -fPIC)
    target_compile_definitions(pynlm PRIVATE -DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
endif()

# Set output name and properties
target_compile_features(pynlm PRIVATE cxx_std_20)

if(WIN32)
    set_target_properties(pynlm PROPERTIES PREFIX "" SUFFIX ".pyd")
else()
    set_target_properties(pynlm PROPERTIES PREFIX "" SUFFIX ".so")
endif()

# Set Python module properties
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "nlm"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Installation configuration
install(TARGETS pynlm
    LIBRARY DESTINATION ${Python_SITEARCH}
    ARCHIVE DESTINATION ${Python_SITEARCH}
    RUNTIME DESTINATION ${Python_SITEARCH}
)

# Install Python module metadata
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../pyproject.toml
    DESTINATION ${Python_SITEARCH}
    RENAME "nlm-${PROJECT_VERSION}.dist-info/pyproject.toml"
)

# Export configuration for CMake package
include(CMakePackageConfigHelpers)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    DESTINATION ${Python_SITEARCH}/nlm/cmake
)

# Development targets
add_custom_target(bindings-info
    COMMAND ${Python_EXECUTABLE} -c "import sys; print('Python executable:', sys.executable)"
    COMMAND ${Python_EXECUTABLE} -c "import pybind11; print('pybind11 version:', pybind11.__version__)"
    COMMENT "Display Python binding information"
)

add_custom_target(validate-bindings
    COMMAND ${Python_EXECUTABLE} -c "import sys; sys.path.insert(0, '${CMAKE_BINARY_DIR}'); import pynlm; print('Successfully imported pynlm module')"
    DEPENDS pynlm
    COMMENT "Validate Python bindings compilation"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Performance and testing configuration
if(TARGET nlm_benchmark)
    add_custom_target(py-benchmark
        COMMAND ${Python_EXECUTABLE} -m pytest benchmarks.py -v
        DEPENDS nlm_benchmark
        COMMENT "Run Python benchmarks"
    )
endif()

message(STATUS "Python bindings configuration complete")
message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "pybind11 version: ${pybind11_VERSION}")