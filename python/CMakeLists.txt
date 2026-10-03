cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Python package version (sync with pyproject.toml)
project_version("0.1.0")

# Configure C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Development mode options
option(PYNLM_BUILD_PYTHON_TESTS "Build Python test suite" ON)
option(PYNLM_BUILD_EXAMPLES "Build example scripts" ON)
option(PYNLM_INSTALL_PYTHON_SUPPORT "Install Python support files" ON)
option(PYNLM_BUILD_DOCS "Build documentation" OFF)

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set Python version requirement
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Core library sources
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    ${NLM_SOURCE_DIR}/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Agent library sources
set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

# World library sources
set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# Create core library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Create agent library
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# Create world library
add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Create Python module
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Configure module properties
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX ""  # pybind11 will add .so/.pyd
    CXX_STANDARD 20
)

# Platform-specific optimizations
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
    target_compile_definitions(pynlm PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)
else()
    target_compile_options(pynlm PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(pynlm PRIVATE -D_GLIBCXX_USE_CXX11_ABI=1)
endif()

# Install the Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Copy supporting files to Python package
if(PYNLM_INSTALL_PYTHON_SUPPORT)
    # Install documentation
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../README.md
        DESTINATION pynlm
        RENAME README.md
    )
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../easy_usage.md
        DESTINATION pynlm
        RENAME EASY_USAGE.md
    )
    
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../HOW_TO_USE.md
        DESTINATION pynlm
        RENAME HOW_TO_USE.md
    )
    
    # Install example scripts
    if(PYNLM_BUILD_EXAMPLES)
        install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/examples.py
            DESTINATION pynlm/examples
            RENAME examples.py
        )
    endif()
endif()

# Python testing
if(PYNLM_BUILD_PYTHON_TESTS)
    enable_testing()
    
    # Add Python tests directory to include path
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    
    # Find pytest
    find_package(Python3Interp REQUIRED)
    
    # Add test target
    add_custom_target(pynlm_python_tests
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_pynlm.py
            --tb=short --verbosity=2
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS pynlm
        COMMENT "Running Python tests for pynlm"
    )
    
    add_test(NAME pynlm_python_tests
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_pynlm.py
            --tb=short --verbosity=0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Development mode configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(pynlm PRIVATE DEBUG NLM_DEBUG)
    target_compile_options(pynlm PRIVATE -O0 -g)
else()
    target_compile_definitions(pynlm PRIVATE NDEBUG)
    target_compile_options(pynlm PRIVATE -O3 -DNDEBUG)
endif()

# Installation summary
if(PYNLM_INSTALL_PYTHON_SUPPORT)
    message(STATUS "Python support files installation enabled")
    if(PYNLM_BUILD_EXAMPLES)
        message(STATUS "Example scripts will be installed")
    endif()
    if(PYNLM_BUILD_PYTHON_TESTS)
        message(STATUS "Python tests will be built")
    endif()
endif()

# Generate version file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.py
    @ONLY
)

# Add configuration options summary
message(STATUS "NLM Python bindings configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python executable: ${Python_EXECUTABLE}")
message(STATUS "  Build Python tests: ${PYNLM_BUILD_PYTHON_TESTS}")
message(STATUS "  Build examples: ${PYNLM_BUILD_EXAMPLES}")
message(STATUS "  Install Python support: ${PYNLM_INSTALL_PYTHON_SUPPORT}")
message(STATUS "  Package version: ${project_version}")
