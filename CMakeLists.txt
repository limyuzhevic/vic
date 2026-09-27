cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find GLEW (for OpenGL extension loading)
find_package(GLEW REQUIRED)

# Find OpenGL and GLFW
find_package(OpenGL REQUIRED)

# Set GLFW search path to include CMake modules and try to find it
set(GLFW_DIR /usr/lib/x86_64-linux-gnu/cmake/glfw3)
find_package(GLFW QUIET)

# Manual GLFW detection (fallback if package config doesn't work)
if(NOT GLFW_FOUND)
    # Include directory
    set(GLFW_INCLUDE_DIR /usr/include)
    
    # Library - try to find it using simpler approach
    find_library(GLFW_LIBRARY
        NAMES glfw.so.3.3 glfw
        PATHS /usr/lib/x86_64-linux-gnu
    )
    
    if(GLFW_LIBRARY)
        set(GLFW_FOUND TRUE)
        set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
        set(GLFW_LIBRARIES ${GLFW_LIBRARY})
    else()
        message(FATAL_ERROR "GLFW not found. Please install libglfw3-dev.")
    endif()
endif()

# Find nlohmann/json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # Try to find it via CMake package
    find_package(nlohmann_json 3.2.0 QUIET)
    if(NOT nlohmann_json_FOUND)
        # Try to download and install it
        include(FetchContent)
        FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.2
        )
        FetchContent_MakeAvailable(nlohmann_json)
    endif()
endif()

# Find yaml-cpp
find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    # Try to find it via CMake package
    find_package(yaml-cpp REQUIRED)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library - only include files that exist
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Config/ConfigParser.cpp
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
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    src/cognition/NeuralPlanner.cpp
    src/cognition/ConceptFormation.cpp
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
    src/motor/Action.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
    # Test program
    src/test_visualization.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config>
    $<INSTALL_INTERFACE:include>
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
)

target_link_libraries(nlm_core PUBLIC
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    nlohmann_json::nlohmann_json
    yaml-cpp
)

# Main executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Testing
enable_testing()
add_subdirectory(tests)
