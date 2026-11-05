cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build type validation
if(NOT CMAKE_BUILD_TYPE MATCHES "Debug|Release|RelWithDebInfo|MinSizeRel")
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Use Debug, Release, RelWithDebInfo, or MinSizeRel.")
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library
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
    src/memory/Memory.cpp
    src/performance/CheckpointSystem.cpp
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
    src/motor/Action.cpp
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
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)

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

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)

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

# Phase 5 demo executable (Phase 5, added here: pattern probe,
# sandbox run, and checkpoint round-trip for the brain-personality
# step — previously Phase 5 was library-only with no demo).
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Phase 6 text/thought demo (new in Phase 6): text-as-a-sense
# association probe, concept/social probes, and a sandbox run with
# a teacher, planner overrides, and a checkpoint round-trip. The old
# nlm_phase6_demo integration demo is left unchanged.
add_executable(nlm_phase6_text_demo
    src/experiments/Phase6TextDemo.cpp
)
target_link_libraries(nlm_phase6_text_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5 nlm_phase6)

# Benchmark executable
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Python bindings with enhanced configuration and better defaults
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module" ON)
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    # Configure Python bindings build with modern CMake practices
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_PRIVATE_HEADERS_INSIDE_NAMESPACE ON
    )
    
    # Add Python-specific compile options
    target_compile_definitions(nlm_core PRIVATE -D_NLM_PYTHON_BINDINGS)
    target_compile_definitions(nlm_agent PRIVATE -D_NLM_PYTHON_BINDINGS)
    target_compile_definitions(nlm_world PRIVATE -D_NLM_PYTHON_BINDINGS)
    
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE
        src
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
    
    # Installation for Python package with proper structure
    install(TARGETS pynlm
        LIBRARY DESTINATION python/pynlm
        ARCHIVE DESTINATION python/pynlm
        RUNTIME DESTINATION python/pynlm
        NAMELINK_SKIP
        NAMELINK_ONLY
    )
    
    # Install Python metadata
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml
        DESTINATION python/pynlm
        RENAME pyproject.toml
    )
endif()

# Enable testing with modern CMake practices
enable_testing()

# Configure test directory with better isolation
add_subdirectory(tests)

# Enhanced test target with better organization
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMENT "Run all tests with verbose output"
    VERBATIM
)

add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L "unit"
    COMMENT "Run unit tests only"
    VERBATIM
)

add_custom_target(run_integration_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L "integration"
    COMMENT "Run integration tests only"
    VERBATIM
)

# Additional configuration options for advanced build scenarios

# Option to build shared libraries instead of static
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

# Installation directory customization
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation prefix")

# Option to enable position independent code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Position independent code")

# Option to build examples
set(BUILD_EXAMPLES ON CACHE BOOL "Build example applications")

# Option to build documentation
set(BUILD_DOCS ON CACHE BOOL "Build documentation")

# Option to enable coverage reporting
set(CMAKE_COVERAGE OFF CACHE BOOL "Enable code coverage reporting")

if(CMAKE_COVERAGE)
    target_compile_options(nlm_core PRIVATE --coverage)
    target_link_options(nlm_core PRIVATE --coverage)
endif()
