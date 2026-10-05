cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Python version detection and validation
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Check Python version compatibility
if(Python_VERSION_VERSION LESS 3.8)
    message(FATAL_ERROR "NLM requires Python 3.8 or higher. Found ${Python_VERSION_VERSION}")
endif()

# C++20 standard with enhanced validation
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable comprehensive warnings
if(MSVC)
    add_compile_options(/W4 /WX)  # Warnings as errors
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
endif()

# Python binding compilation settings
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Core library with modern CMake practices (same as main project)
add_library(nlm_core STATIC
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
    
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
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
    ${NLM_SOURCE_DIR}/environment/Action.cpp
    
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Agent system library
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system library
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Create Python module with enhanced configuration
pybind11_add_module(pynlm bindings.cpp)

target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Set Python module properties
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Set Python module properties
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Python package configuration
set(PYTHON_PACKAGE_DIR "${CMAKE_INSTALL_PREFIX}/lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages")

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION ${PYTHON_PACKAGE_DIR}
    ARCHIVE DESTINATION ${PYTHON_PACKAGE_DIR}
    RUNTIME DESTINATION ${PYTHON_PACKAGE_DIR}
)

# Install Python package metadata
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml
    DESTINATION ${PYTHON_PACKAGE_DIR}
)

# Python development package configuration
if(PYTHON_PACKAGE_DEV_INSTALL)
    # Install development headers and configuration
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../src/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
    )
    
    install(EXPORT pynlmTargets
        FILE pynlmTargets.cmake
        NAMESPACE pynlm::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pynlm
    )
endif()

# Python-specific build options
option(PYTHON_BUILD_TESTS "Build Python tests" ON)
option(PYTHON_INSTALL_DEVEL "Install development files" ON)
option(PYTHON_ENABLE_DEBUG_SYMBOLS "Enable debug symbols for Python module" OFF)

# Add Python-specific build targets
add_custom_target(python_build
    COMMAND ${Python_EXECUTABLE} -c "import pybind11; print('pybind11 found: ' + str(pybind11.__version__))"
    COMMENT "Validating Python environment"
    VERBATIM
)

add_custom_target(python_test
    COMMAND ${Python_EXECUTABLE} -m pytest
    DEPENDS pynlm
    COMMENT "Running Python tests"
    VERBATIM
)

# Generate Python package metadata
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml.in
    ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
    @ONLY
)

# Add comprehensive Python package information
message(STATUS "Python bindings configuration:")
message(STATUS "  Python executable: ${Python_EXECUTABLE}")
message(STATUS "  Python version: ${Python_VERSION_STRING}")
message(STATUS "  pybind11: ${pybind11_VERSION_STRING}")
message(STATUS "  Build tests: ${PYTHON_BUILD_TESTS}")
message(STATUS "  Install development: ${PYTHON_INSTALL_DEVEL}")
message(STATUS "  Debug symbols: ${PYTHON_ENABLE_DEBUG_SYMBOLS}")

# Set Python module output directory
set(PYTHON_MODULE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/python")
set_target_properties(pynlm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PYTHON_MODULE_OUTPUT_DIR}"
)

# Add helper targets for Python development
add_custom_target(python_install
    COMMAND ${Python_EXECUTABLE} -m pip install -e .
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Install Python package in development mode"
    VERBATIM
)

add_custom_target(python_uninstall
    COMMAND ${Python_EXECUTABLE} -m pip uninstall -y pynlm
    COMMENT "Uninstall Python package"
    VERBATIM
)