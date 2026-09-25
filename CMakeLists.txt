# NLM Python Bindings Build Configuration

# Build the C++ extension module for Python
toolchain_find(PYTHON_EXECUTABLE REQUIRED)

# Find pybind11
find_package(pybind11 REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project name and version
project(pynlm VERSION 2.0.0 LANGUAGES CXX)

# Create the Python extension module
set(PYNLM_SOURCES
    pynlm/_pynlm.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralRegion.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/Brain.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/Error/Error.cpp
    src/core/Error/ErrorUtilities.cpp
    src/core/Factory/Factory.cpp
    src/dynamics/SpikeSystem.cpp
    src/plasticity/STDP.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/StructuralPlasticity.cpp
    src/neuromodulation/Dopamine.cpp
    src/neuromodulation/Curiosity.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/PredictionError.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/memory/NeuralAssociativeMemory.cpp
    src/cognition/NeuralPlanner.cpp
    src/cognition/ConceptFormation.cpp
    src/cognition/AttentionalSelection.cpp
    src/development/DevelopmentSystem.cpp
    src/development/Maturation.cpp
    src/development/Pruning.cpp
    src/development/Synaptogenesis.cpp
    src/world/SimpleWorld.cpp
    src/world/Observation.cpp
    src/agent/AgentBrain.cpp
    src/agent/SensoryPercept.cpp
    src/agent/AgentBody.cpp
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/visualization/VisualizationInterface.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Phase3Experiment.cpp
    src/experiments/Phase4Experiment.cpp
    src/experiments/Phase5Experiment.cpp
    src/experiments/Phase6IntegratedExperiment.cpp
)

# Add the Python module
pybind11_add_module(_pynlm ${PYNLM_SOURCES})

# Target properties
set_target_properties(_pynlm PROPERTIES
    PREFIX ""
    OUTPUT_NAME "pynlm._pynlm"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Include directories
target_include_directories(_pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pybind11_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(_pynlm PRIVATE
    ${PYTHON_LIBRARIES}
)

# Compiler options
target_compile_options(_pynlm PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -O3
    -march=native
    -fPIC
)

# Installation
install(TARGETS _pynlm
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}
    RUNTIME DESTINATION bin
)

# Install Python package files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm/__init__.py
    DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm/_pynlm.cpp
    DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
)

# Create a setup.py for pip installation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/setup.py
    @ONLY
)

# Create a requirements.txt for Python dependencies
file(WRITE
    ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    "pybind11>=2.11.0\n"
    "numpy>=1.20\n"
    "pytest>=7.0\n"
)

# Create documentation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md.in
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    @ONLY
)
