cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Use the existing NLM core libraries from the parent build
# This avoids duplicating build work and ensures consistency

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Find the existing NLM libraries
# They should be built as part of the main build
# If not, we'll build them here

# Try to find the libraries first
find_library(NLM_CORE_LIB nlm_core PATHS "${CMAKE_BINARY_DIR}/lib" NO_DEFAULT_PATH)
find_library(NLM_AGENT_LIB nlm_agent PATHS "${CMAKE_BINARY_DIR}/lib" NO_DEFAULT_PATH)
find_library(NLM_WORLD_LIB nlm_world PATHS "${CMAKE_BINARY_DIR}/lib" NO_DEFAULT_PATH)

if (NOT NLM_CORE_LIB OR NOT NLM_AGENT_LIB OR NOT NLM_WORLD_LIB)
    # If libraries not found, build them here
    # This is a simplified version - in a real project, you'd want
    # to use the existing CMake configuration
    
    # Core library sources (only what's needed for Python bindings)
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
    
    add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    
    # Agent library
    set(NLM_AGENT_SOURCES
        ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
        ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
    )
    
    add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
    target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(nlm_agent PUBLIC nlm_core)
    
    # World library
    set(NLM_WORLD_SOURCES
        ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
    )
    
    add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
    target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_link_libraries(nlm_world PUBLIC nlm_core)
endif()

# Create the Python module
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)