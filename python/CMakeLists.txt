cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Enable C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Find or build the core libraries from the main project
# Check if nlm_core target already exists (when building with setup.py)
if(NOT TARGET nlm_core)
    message(STATUS "Building NLM core libraries for Python bindings")
    
    # Core library - similar to what's in the main CMakeLists.txt but minimal
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
    
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_options(nlm_core PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    
    # Agent system
    add_library(nlm_agent STATIC
        ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
        ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
    )
    target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
    target_link_libraries(nlm_agent PUBLIC nlm_core)
    
    # World system  
    add_library(nlm_world STATIC
        ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
    )
    target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
    target_link_libraries(nlm_world PUBLIC nlm_core)
else()
    message(STATUS "Using existing NLM core libraries: nlm_core, nlm_agent, nlm_world")
endif()

# Build the Python module
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Set output directory to match Python package structure
set_target_properties(pynlm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pynlm
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pynlm
)

# Install targets
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)