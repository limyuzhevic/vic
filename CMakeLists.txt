cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Building NLM in ${CMAKE_BUILD_TYPE} mode")

# Enable warnings and static analysis
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Python dependencies for bindings
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library with target-based dependencies
add_library(nlm_core STATIC
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Brain components
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity systems
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Development systems
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
    
    # Memory systems
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
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor system
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

# Configure core library
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Compiler optimization settings based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(nlm_core PRIVATE -g -O0 -DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nlm_core PRIVATE -O3 -DNDEBUG -march=native)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(nlm_core PRIVATE -O2 -g -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    target_compile_options(nlm_core PRIVATE -Os -DNDEBUG)
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

# Phase 3 experiments (basic learning)
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments (social learning)
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 5 experiments (performance optimization)
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 experiments (Final Integration)
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Main executable (Phase 6 Final Integration)
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

# Phase 5 demo executable
add_executable(nlm_phase5_demo
    src/experiments/Phase5Experiment.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Phase 6 demo executable (Phase 6 Final Integration)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6)

# Python bindings (pynlm)
add_library(pynlm_bindings MODULE
    python/bindings.cpp
)
set_target_properties(pynlm_bindings PROPERTIES
    OUTPUT_NAME "pynlm"
    PREFIX ""
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)
target_include_directories(pynlm_bindings PRIVATE
    python
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(pynlm_bindings PRIVATE nlm_core nlm_agent nlm_world)

# Python package installation
target_include_directories(pynlm_bindings PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/python>
    $<INSTALL_INTERFACE:include/pynlm>
)

# Testing infrastructure
enable_testing()

# Add test directory
add_subdirectory(tests)

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install Python bindings
install(TARGETS pynlm_bindings
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
    ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
    RUNTIME DESTINATION ${PYTHON_SITE_PACKAGES}/pynlm
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING "*.hpp" "*.h"
)

# Configure package for find_package
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create and install export configuration
export(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    FILE ${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/NLM
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION lib/cmake/NLM
)

# Provide help for common build targets
add_custom_target(help_all
    COMMAND echo "Available build targets:"
    COMMAND echo "  all - Build all libraries and executables"
    COMMAND echo "  core - Build core library only"
    COMMAND echo "  agent - Build agent library only"
    COMMAND echo "  world - Build world library only"
    COMMAND echo "  phase3 - Build phase3 experiments"
    COMMAND echo "  phase4 - Build phase4 experiments"
    COMMAND echo "  phase5 - Build phase5 experiments"
    COMMAND echo "  phase6 - Build phase6 experiments"
    COMMAND echo "  demos - Build all demo executables"
    COMMAND echo "  python - Build Python bindings"
    COMMAND echo "  tests - Run tests"
    COMMENT "Show available build targets"
)

add_custom_target(docs
    COMMAND echo "Documentation available in:"
    COMMAND echo "  docs/ - Architecture documentation"
    COMMAND echo "  easy_usage.md - Beginner user guide"
    COMMAND echo "  advanced_user_guide.md - Advanced user guide"
    COMMAND echo "  HOW_TO_USE.md - Usage instructions"
    COMMAND echo "  README.md - Project overview"
    COMMENT "Show documentation locations"
)

# Generate compile_commands.json for tooling
target_compile_features(nlm_core PRIVATE cxx_std_20)

# Summary output
message(STATUS "NLM Project Configuration:")
message(STATUS "  C++ Standard: C++20")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Python Available: ${Python_FOUND}")
message(STATUS "  pybind11 Available: ${pybind11_FOUND}")
message(STATUS "  Core library: nlm_core")
message(STATUS "  Agent library: nlm_agent")
message(STATUS "  World library: nlm_world")
message(STATUS "  Phase 3 library: nlm_phase3")
message(STATUS "  Phase 4 library: nlm_phase4")
message(STATUS "  Phase 5 library: nlm_phase5")
message(STATUS "  Phase 6 library: nlm_phase6")
message(STATUS "  Python bindings: pynlm_bindings")
message(STATUS "  Executables: nlm, nlm_phase3_demo, nlm_phase4_demo, nlm_phase5_demo, nlm_phase6_demo")

# Provide help for common build targets
add_custom_target(help_all
    COMMAND echo "Available build targets:"
    COMMAND echo "  all - Build all libraries and executables"
    COMMAND echo "  core - Build core library only"
    COMMAND echo "  agent - Build agent library only"
    COMMAND echo "  world - Build world library only"
    COMMAND echo "  phase3 - Build phase3 experiments"
    COMMAND echo "  phase4 - Build phase4 experiments"
    COMMAND echo "  phase5 - Build phase5 experiments"
    COMMAND echo "  phase6 - Build phase6 experiments"
    COMMAND echo "  demos - Build all demo executables"
    COMMAND echo "  python - Build Python bindings"
    COMMAND echo "  tests - Run tests"
    COMMENT "Show available build targets"
)