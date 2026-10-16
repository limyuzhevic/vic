cmake_minimum_required(VERSION 3.16)

project(nlm_brain VERSION 1.0.0 LANGUAGES CXX)

# Brain library sources

set(BRAIN_SOURCES
    Brain.cpp
    BrainSimulationManager.cpp
    BrainDevelopmentManager.cpp
    BrainNeuromodulationManager.cpp
    BrainCognitiveManager.cpp
    BrainMemoryManager.cpp
    BrainPlasticityManager.cpp
    BrainNeuralCore.cpp
    Synapse.cpp
    Neuron.cpp
    NeuralRegion.cpp
    NeuralPopulation.cpp
)

# Add brain library
add_library(nlm_brain STATIC ${BRAIN_SOURCES})

target_include_directories(nlm_brain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../brain
)

target_link_libraries(nlm_brain PUBLIC nlm_core nlohmann_json::nlohmann_json yaml-cpp)

# Install the brain library
install(TARGETS nlm_brain
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../brain/ DESTINATION include/nlm/brain
    FILES_MATCHING PATTERN "*.hpp"
)
