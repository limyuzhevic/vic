cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Enable C++20 with modern standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure build type with sensible defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Building NLM (熙然) version ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Modern warning flags for different compilers
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wno-missing-field-initializers -Wno-unused-parameter)
    endif()
endif()

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/configs")

# Configuration for external dependencies
find_package(PkgConfig QUIET)

# Find nlohmann/json for JSON configuration
find_package(nlohmann_json 3.11.2 QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found, building from source...")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
else()
    message(STATUS "Found nlohmann/json: ${nlohmann_json_VERSION}")
endif()

# Find yaml-cpp for YAML configuration
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    message(STATUS "yaml-cpp not found, building from source...")
    include(FetchContent)
    FetchContent_Declare(
        yaml_cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-0.7.0
    )
    FetchContent_MakeAvailable(yaml_cpp)
else()
    message(STATUS "Found yaml-cpp: ${YAML_CPP_VERSION}")
endif()

# Find Python for bindings
find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

# Enable testing framework
enable_testing()
set(TEST_TIMEOUT 60)  # Timeout for individual tests

# Create component libraries for better organization

# Core utilities library
add_library(nlm_core_utils STATIC
    src/core/Types/Types.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
)
target_include_directories(nlm_core_utils PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/core>
    $<INSTALL_INTERFACE:include/nlm/core>
)
add_library(nlm::core_utils ALIAS nlm_core_utils)

# Brain computation library
add_library(nlm_brain STATIC
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)
target_include_directories(nlm_brain PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/brain>
    $<INSTALL_INTERFACE:include/nlm/brain>
)
target_link_libraries(nlm_brain PUBLIC nlm_core_utils)
add_library(nlm::brain ALIAS nlm_brain)

# Configuration library
add_library(nlm_config STATIC
    src/core/Config/Config.cpp
)
target_include_directories(nlm_config PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/core>
    $<INSTALL_INTERFACE:include/nlm/core>
)
target_link_libraries(nlm_config PUBLIC nlohmann_json::nlohmann_json)
add_library(nlm::config ALIAS nlm_config)

# Neural dynamics library
add_library(nlm_dynamics STATIC
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)
target_include_directories(nlm_dynamics PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/dynamics>
    $<INSTALL_INTERFACE:include/nlm/dynamics>
)
target_link_libraries(nlm_dynamics PUBLIC nlm_brain)
add_library(nlm::dynamics ALIAS nlm_dynamics)

# Plasticity library
add_library(nlm_plasticity STATIC
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)
target_include_directories(nlm_plasticity PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/plasticity>
    $<INSTALL_INTERFACE:include/nlm/plasticity>
)
target_link_libraries(nlm_plasticity PUBLIC nlm_dynamics)
add_library(nlm::plasticity ALIAS nlm_plasticity)

# Development library
add_library(nlm_development STATIC
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)
target_include_directories(nlm_development PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/development>
    $<INSTALL_INTERFACE:include/nlm/development>
)
target_link_libraries(nlm_development PUBLIC nlm_plasticity)
add_library(nlm::development ALIAS nlm_development)

# Neuromodulation library
add_library(nlm_neuromodulation STATIC
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)
target_include_directories(nlm_neuromodulation PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/neuromodulation>
    $<INSTALL_INTERFACE:include/nlm/neuromodulation>
)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_dynamics)
add_library(nlm::neuromodulation ALIAS nlm_neuromodulation)

# Memory systems library
add_library(nlm_memory STATIC
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)
target_include_directories(nlm_memory PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/memory>
    $<INSTALL_INTERFACE:include/nlm/memory>
)
target_link_libraries(nlm_memory PUBLIC nlm_brain nlm_plasticity)
add_library(nlm::memory ALIAS nlm_memory)

# Prediction library
add_library(nlm_prediction STATIC
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)
target_include_directories(nlm_prediction PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/prediction>
    $<INSTALL_INTERFACE:include/nlm/prediction>
)
target_link_libraries(nlm_prediction PUBLIC nlm_memory nlm_neuromodulation)
add_library(nlm::prediction ALIAS nlm_prediction)

# Cognitive systems library
add_library(nlm_cognition STATIC
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)
target_include_directories(nlm_cognition PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/cognition>
    $<INSTALL_INTERFACE:include/nlm/cognition>
)
target_link_libraries(nlm_cognition PUBLIC nlm_prediction nlm_memory)
add_library(nlm::cognition ALIAS nlm_cognition)

# Sensory systems library
add_library(nlm_sensory STATIC
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)
target_include_directories(nlm_sensory PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/sensory>
    $<INSTALL_INTERFACE:include/nlm/sensory>
)
target_link_libraries(nlm_sensory PUBLIC nlm_brain)
add_library(nlm::sensory ALIAS nlm_sensory)

# Motor systems library
add_library(nlm_motor STATIC
    src/motor/MotorSystem.cpp
    src/motor/Action.cpp
)
target_include_directories(nlm_motor PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/motor>
    $<INSTALL_INTERFACE:include/nlm/motor>
)
target_link_libraries(nlm_motor PUBLIC nlm_cognition)
add_library(nlm::motor ALIAS nlm_motor)

# Environment systems library
add_library(nlm_environment STATIC
    src/environment/Environment.cpp
    src/environment/Observation.cpp
)
target_include_directories(nlm_environment PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/environment>
    $<INSTALL_INTERFACE:include/nlm/environment>
)
target_link_libraries(nlm_environment PUBLIC nlm_sensory nlm_motor)
add_library(nlm::environment ALIAS nlm_environment)

# Agent systems (final integration)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/agent>
    $<INSTALL_INTERFACE:include/nlm/agent>
)
target_link_libraries(nlm_agent PUBLIC nlm_brain nlm_sensory nlm_motor nlm_environment)
add_library(nlm::agent ALIAS nlm_agent)

# Experiment systems library
add_library(nlm_experiments STATIC
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)
target_include_directories(nlm_experiments PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/experiments>
    $<INSTALL_INTERFACE:include/nlm/experiments>
)
target_link_libraries(nlm_experiments PUBLIC nlm_agent nlm_config)
add_library(nlm::experiments ALIAS nlm_experiments)

# Visualization library
add_library(nlm_visualization STATIC
    src/visualization/VisualizationInterface.cpp
)
target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/visualization>
    $<INSTALL_INTERFACE:include/nlm/visualization>
)
add_library(nlm::visualization ALIAS nlm_visualization)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/experiments>
)
target_link_libraries(nlm_phase3 PUBLIC nlm_brain nlm_agent nlm_world)
add_library(nlm::phase3 ALIAS nlm_phase3)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/experiments>
)
target_link_libraries(nlm_phase4 PUBLIC nlm_brain nlm_agent nlm_world nlm_phase3)
add_library(nlm::phase4 ALIAS nlm_phase4)

# Phase 6 integration experiment
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}/experiments>
)
target_link_libraries(nlm_phase6 PUBLIC nlm_brain nlm_agent nlm_world nlm_experiments)
add_library(nlm::phase6 ALIAS nlm_phase6)

# Main simulation executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_brain nlm_config nlm_experiments)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_brain nlm_agent nlm_world nlm_phase6)

# Python bindings executable
add_executable(pynlm_module
    python/bindings.cpp
)
target_link_libraries(pynlm_module PUBLIC pybind11::module nlm_brain nlm_config nlm_agent nlm_world nlm_experiments)

# Installation targets
install(TARGETS nlm
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install headers
install(DIRECTORY src/core/
    DESTINATION include/nlm/core
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/brain/
    DESTINATION include/nlm/brain
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/dynamics/
    DESTINATION include/nlm/dynamics
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/plasticity/
    DESTINATION include/nlm/plasticity
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/development/
    DESTINATION include/nlm/development
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/neuromodulation/
    DESTINATION include/nlm/neuromodulation
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/memory/
    DESTINATION include/nlm/memory
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/prediction/
    DESTINATION include/nlm/prediction
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/cognition/
    DESTINATION include/nlm/cognition
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/sensory/
    DESTINATION include/nlm/sensory
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/motor/
    DESTINATION include/nlm/motor
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/environment/
    DESTINATION include/nlm/environment
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/agent/
    DESTINATION include/nlm/agent
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY src/experiments/
    DESTINATION include/nlm/experiments
    FILES_MATCHING PATTERN "*.hpp"
)

# Install configuration files
install(DIRECTORY ${NLM_CONFIG_DIR}/
    DESTINATION etc/nlm/configs
    FILES_MATCHING PATTERN "*.cfg" PATTERN "*.json" PATTERN "*.yaml"
)

# Install documentation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/easy_usage.md
    ${CMAKE_CURRENT_SOURCE_DIR}/HOW_TO_USE.md
    DESTINATION share/doc/nlm
)

# Installation for Python bindings
install(FILES python/bindings.cpp
    DESTINATION include/nlm/python
)

# Configure package for find_package
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION lib/cmake/nlm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION lib/cmake/nlm
)

# Add test targets
add_subdirectory(tests)

# Add custom targets for common operations
add_custom_target(config_examples
    COMMAND ${CMAKE_COMMAND} -E echo "Configuration examples available in:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - ${NLM_CONFIG_DIR}/default.cfg (simple)"
    COMMAND ${CMAKE_COMMAND} -E echo "  - ${NLM_CONFIG_DIR}/nlm_config.json (JSON)"
    COMMAND ${CMAKE_COMMAND} -E echo "  - ${NLM_CONFIG_DIR}/nlm_config.yaml (YAML)"
    COMMENT "Show configuration examples"
)

add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation available in:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - README.md (overview)"
    COMMAND ${CMAKE_COMMAND} -E echo "  - easy_usage.md (beginner guide)"
    COMMAND ${CMAKE_COMMAND} -E echo "  - HOW_TO_USE.md (detailed guide)"
    COMMENT "Show documentation locations"
)

# Print configuration summary
message(STATUS "NLM Configuration:")
message(STATUS "  - Core library: nlm_core_utils")
message(STATUS "  - Brain library: nlm_brain")
message(STATUS "  - Configuration library: nlm_config (JSON/YAML support)")
message(STATUS "  - Development library: nlm_development")
message(STATUS "  - Memory library: nlm_memory")
message(STATUS "  - Cognition library: nlm_cognition")
message(STATUS "  - Main executable: nlm (basic simulation)")
message(STATUS "  - Phase 6 demo: nlm_phase6_demo (integration test)")
message(STATUS "  - Python bindings: pynlm_module")
message(STATUS "  - Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  - all: Build all libraries and executables")
message(STATUS "  - tests: Run unit tests")
message(STATUS "  - config_examples: Show configuration file examples")
message(STATUS "  - docs: Show documentation locations")
message(STATUS "")
message(STATUS "To build and install:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake .. -DCMAKE_BUILD_TYPE=Release")
message(STATUS "  make -j$(nproc)")
message(STATUS "  sudo make install")
