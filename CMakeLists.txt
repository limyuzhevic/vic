cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
project_source_dir(${CMAKE_CURRENT_SOURCE_DIR})

# Helper function to add an NLM library
define_property(GLOBAL PROPERTY NLM_SOURCE_DIR BRIEF_DOCS "Source directory for NLM project" FULL_DOCS "Source directory for NLM project (used by all build files)")

function(add_nlm_library target_name target_sources)
    add_library(${target_name} STATIC ${target_sources})
    target_include_directories(${target_name} PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
    # Set target properties for consistent behavior
    set_target_properties(${target_name} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endfunction()

function(link_nlm_target target_name)
    if(${ARGC} GREATER_EQUAL 1)
        target_link_libraries(${target_name} PUBLIC ${ARGV0})
    endif()
endfunction()

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

# Phase 3 experiment library sources
set(NLM_PHASE3_SOURCES
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

# Phase 4 experiment library sources
set(NLM_PHASE4_SOURCES
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

# Create core library using helper function
add_nlm_library(nlm_core ${NLM_CORE_SOURCES})

# Create agent library
add_nlm_library(nlm_agent ${NLM_AGENT_SOURCES})
link_nlm_target(nlm_agent nlm_core)

# Create world library
add_nlm_library(nlm_world ${NLM_WORLD_SOURCES})
link_nlm_target(nlm_world nlm_core)

# Create Phase 3 experiment library
add_nlm_library(nlm_phase3 ${NLM_PHASE3_SOURCES})
link_nlm_target(nlm_phase3 nlm_core nlm_agent nlm_world)

# Create Phase 4 experiment library
add_nlm_library(nlm_phase4 ${NLM_PHASE4_SOURCES})
link_nlm_target(nlm_phase4 nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    ${NLM_SOURCE_DIR}/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)