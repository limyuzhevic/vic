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

# Feature options for customization
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_EXAMPLES "Build example applications" ON)
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_VISUALIZATION "Build visualization tools" OFF)
option(NLM_ENABLE_JSON "Enable JSON configuration support" ON)
option(NLM_ENABLE_ADVANCED_MEMORY "Enable advanced memory systems" ON)
option(NLM_ENABLE_EXPERIMENTS "Enable experiment frameworks" ON)

# Performance optimization options
option(NLM_OPTIMIZE_PERF "Enable performance optimizations" ON)
option(NLM_ENABLE_SIMD "Enable SIMD optimizations" ON)
option(NLM_ENABLE_PARALLEL "Enable parallel processing" OFF)
option(NLM_ENABLE_VECTORIZATION "Enable vectorization" ON)

# Debug and development options
option(NLM_DEBUG_LOGS "Enable detailed debug logging" OFF)
option(NLM_DEVELOPMENT_FEATURES "Enable development features" OFF)

# Set default values for Phase 2
if(NLM_ENABLE_JSON)
    message(STATUS "JSON configuration support enabled")
endif()

if(NLM_ENABLE_ADVANCED_MEMORY)
    message(STATUS "Advanced memory systems enabled")
endif()

if(NLM_BUILD_PYTHON_BINDINGS)
    message(STATUS "Python bindings enabled")
endif()

# Performance settings
if(NLM_OPTIMIZE_PERF)
    add_compile_definitions(NLM_PERFORMANCE_OPTIMIZED)
    if(NLM_ENABLE_SIMD)
        add_compile_definitions(NLM_SSE4_INTRINSICS)
    endif()
endif()

# Debug settings
if(NLM_DEBUG_LOGS)
    add_compile_definitions(NLM_DEBUG_LOGS_ENABLED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Development features
if(NLM_DEVELOPMENT_FEATURES)
    add_compile_definitions(NLM_DEVELOPMENT_MODE)
endif()

# nlohmann/json dependency (header-only JSON library for Modern C++)
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library (always built)
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
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

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${nlohmann_json_SOURCE_DIR}/include>
)

# Add performance definitions if requested
if(NLM_OPTIMIZE_PERF)
    target_compile_definitions(nlm_core PRIVATE NLM_PERFORMANCE_OPTIMIZED)
    if(NLM_ENABLE_SIMD)
        target_compile_definitions(nlm_core PRIVATE NLM_SSE4_INTRINSICS)
    endif()
endif()

if(NLM_DEBUG_LOGS)
    target_compile_definitions(nlm_core PRIVATE NLM_DEBUG_LOGS_ENABLED)
endif()

if(NLM_DEVELOPMENT_FEATURES)
    target_compile_definitions(nlm_core PRIVATE NLM_DEVELOPMENT_MODE)
endif()

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
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

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

# Python bindings (conditional)
if(NLM_BUILD_PYTHON_BINDINGS)
    # Find Python and pybind11
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    
    # Try to find pybind11
    find_package(pybind11 QUIET)
    if(NOT pybind11_FOUND)
        message(STATUS "pybind11 not found, downloading...")
        include(FetchContent)
        FetchContent_Declare(
            pybind11
            URL https://github.com/pybind/pybind11/releases/download/v2.11.1/pybind11-2.11.1.tar.gz
        )
        FetchContent_MakeAvailable(pybind11)
    endif()
    
    # Python bindings library
    add_library(pynlm MODULE
        python/bindings.cpp
    )
    
    target_include_directories(pynlm PRIVATE
        python/
        ${nlohmann_json_SOURCE_DIR}/include
        ${pybind11_INCLUDE_DIRS}
        src/
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core)
    
    # Set module properties
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        OUTPUT_NAME "pynlm"
        CXX_STANDARD 20
    )
    
    if(WIN32)
        set_target_properties(pynlm PROPERTIES SUFFIX ".pyd")
    endif()
    
    message(STATUS "Python bindings enabled")
endif()

# Example applications (conditional)
if(NLM_BUILD_EXAMPLES)
    # Advanced command-line interface
    add_executable(nlm_cli
        examples/nlm_cli.cpp
    )
    target_link_libraries(nlm_cli PUBLIC nlm_core)
    
    # Configuration editor
    add_executable(nlm_config_editor
        examples/nlm_config_editor.cpp
    )
    target_link_libraries(nlm_config_editor PUBLIC nlm_core)
    
    # Batch processing tool
    add_executable(nlm_batch
        examples/nlm_batch.cpp
    )
    target_link_libraries(nlm_batch PUBLIC nlm_core)
    
    message(STATUS "Example applications enabled")
endif()

# Visualization tools (conditional)
if(NLM_BUILD_VISUALIZATION)
    # Neural network visualizer
    add_executable(nlm_visualizer
        visualization/nlm_visualizer.cpp
    )
    target_link_libraries(nlm_visualizer PUBLIC nlm_core)
    
    # Activity monitor
    add_executable(nlm_monitor
        visualization/nlm_monitor.cpp
    )
    target_link_libraries(nlm_monitor PUBLIC nlm_core)
    
    message(STATUS "Visualization tools enabled")
endif()

# Experiment frameworks (conditional)
if(NLM_ENABLE_EXPERIMENTS)
    # Experiment runner
    add_library(nlm_experiments STATIC
        experiments/AdvancedExperimentRunner.cpp
        experiments/ExperimentAnalyzer.cpp
    )
    target_link_libraries(nlm_experiments PUBLIC nlm_core)
    
    # Phase 6 demo
    add_executable(nlm_phase6_demo
        experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_experiments)
    
    message(STATUS "Experiment frameworks enabled")
endif()

# Testing (conditional)
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)