cmake_minimum_required(VERSION 3.16)
project(NLMCli VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find dependencies
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/brain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/agent
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cognition
    ${CMAKE_CURRENT_SOURCE_DIR}/src/development
    ${CMAKE_CURRENT_SOURCE_DIR}/src/neuromodulation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/prediction
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/src/experiments
    ${CMAKE_CURRENT_SOURCE_DIR}/src/environment
    ${CMAKE_CURRENT_SOURCE_DIR}/src/motor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plasticity
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensory
    ${CMAKE_CURRENT_SOURCE_DIR}/src/visualization
    ${CMAKE_CURRENT_SOURCE_DIR}/src/world
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dynamics
)

# Core NLM library
add_library(nlm-core STATIC
    src/core/Config/Config.cpp
    src/core/Config/Config.cpp
    src/core/Logger/Logger.cpp
    src/core/Random/Random.cpp
    src/core/SimulationClock/SimulationClock.cpp
)

# Brain library
add_library(nlm-brain STATIC
    src/brain/Brain.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralRegion.cpp
    src/brain/NeuralPopulation.cpp
)

# Agent library
add_library(nlm-agent STATIC
    src/agent/AgentBrain.cpp
    src/agent/SensoryPercept.cpp
)

# Create CLI executable
add_executable(nlm-cli
    src/nlm-cli.cpp
    src/cli/NLMCLI.cpp
)

# Link libraries
target_link_libraries(nlm-cli PRIVATE nlohmann_json::nlohmann_json nlm-core nlm-brain nlm-agent)

# Set output name
set_target_properties(nlm-cli PROPERTIES OUTPUT_NAME "nlm-cli")

# Installation
install(TARGETS nlm-cli DESTINATION bin)
install(DIRECTORY configs/ DESTINATION configs)

# Add directories for brain and checkpoint files
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/brains)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/checkpoints)

# Add custom targets for managing the CLI
add_custom_target(create_brain_dirs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/brains
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/checkpoints
)

# Add version info
target_compile_definitions(nlm-cli PRIVATE "NLM_VERSION=\"${PROJECT_VERSION}\"")