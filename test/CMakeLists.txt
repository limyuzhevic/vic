cmake_minimum_required(VERSION 3.16)
project(ConceptFormationIntegrationTest LANGUAGES CXX)

# Test the concept formation integration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create test executable
add_executable(test_concept_integration test_concept_integration.cpp
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)

# Link with the nlm core library
add_library(nlm_core STATIC
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
)

# Include directories
target_include_directories(test_concept_integration PRIVATE
    src
)

target_link_libraries(test_concept_integration nlm_core)