cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type options
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Build options
option(BUILD_TESTS "Build test executables" ON)
option(BUILD_EXAMPLES "Build demo executables" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_DOCUMENTATION "Build documentation" OFF)
option(ENABLE_DEBUG_LOGS "Enable debug logging output" OFF)
option(ENABLE_PERFORMANCE_COUNTERS "Enable performance counters" ON)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(INSTALL_PROJECT "Generate installation rules" ON)

# Advanced configuration options
option(ENABLE_ADVANCED_CONFIG "Enable advanced configuration features" ON)
option(ENABLE_RUNTIME_CONFIG "Enable runtime configuration changes" ON)
option(ENABLE_CONFIG_VALIDATION "Enable configuration validation" ON)
option(ENABLE_CONFIG_TEMPLATES "Enable configuration templates" ON)

# Performance optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2 /fp:fast /arch:AVX2)
    else()
        add_compile_options(-O3 -march=native -mtune=native -flto)
        add_link_options(-flto)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT ENABLE_DEBUG_LOGS)
        add_compile_options(-DNDEBUG)
    endif()
endif()

# Core library
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Config/ConfigExtended.cpp
    src/core/Config/ValidationSystem.cpp
    src/core/Config/SchemaValidator.cpp
    src/core/Config/RuntimeConfig.cpp
    src/core/Config/ConfigurationManager.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # Brain
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    # Plasticity
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    # Development
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    # Neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    # Memory
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    # Sensory
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    # Motor
    src/motor/MotorSystem.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

if(BUILD_SHARED_LIBS)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLES)
    # Phase 3 demo executable
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

    # Phase 4 demo executable
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

    # Phase 6 demo executable (comprehensive integration test)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

if(ENABLE_PERFORMANCE_COUNTERS)
    target_compile_definitions(nlm_core PUBLIC NLM_PERFORMANCE_ENABLED)
    target_compile_definitions(nlm)
    target_compile_definitions(nlm_phase3_demo)
    target_compile_definitions(nlm_phase4_demo)
    target_compile_definitions(nlm_phase6_demo)
endif()

if(ENABLE_DEBUG_LOGS)
    target_compile_definitions(nlm_core PUBLIC NLM_DEBUG_ENABLED)
    target_compile_definitions(nlm)
    target_compile_definitions(nlm_phase3_demo)
    target_compile_definitions(nlm_phase4_demo)
    target_compile_definitions(nlm_phase6_demo)
endif()

if(INSTALL_PROJECT)
    install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
    
    install(DIRECTORY src/
        DESTINATION include/nlm
        FILES_MATCHING PATTERN "*.hpp"
    )
    
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/python/bindings.cpp
        DESTINATION include/nlm/python
    )
endif()
