cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard with modern best practices
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type with sensible defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# Enable comprehensive warnings with additional flags
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-DNOMINMAX)  # Prevent min/max macro conflicts
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type -Werror=uninitialized -Werror=overloaded-virtual -Werror=shadow)
    add_definitions(-DNOMINMAX -D_X86_64_ -Wno-implicit-fallthrough)
endif()

# Optimization and debugging flags
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
if(BUILD_TYPE_UPPER STREQUAL "DEBUG")
    add_compile_options(-g3 -O0 -DDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "RELEASE")
    add_compile_options(-O3 -DNDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    add_compile_options(-O2 -g -DNDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
    add_compile_options(-Os -DNDEBUG)
else()
    add_compile_options(-O3 -DNDEBUG)
endif()

# Position independent code for better linking
if(NOT MSVC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library with better structure and modern CMake practices
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
    
    # Neural dynamics and computation
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity mechanisms
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Development system
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
    
    # Cognitive functions
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction and planning
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor control
    src/motor/MotorSystem.cpp
    
    # Environment simulation
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiment framework
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

# Modern target properties for better CMake integration
target_compile_features(nlm_core PUBLIC cxx_std_20)
target_compile_options(nlm_core PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang>:-fPIC -fvisibility=hidden>)

# Include directories for core library
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (brain-world interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
    src/agent/AgentBody.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment and simulation)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/Observation.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_world PUBLIC nlm_core)

# Performance optimization libraries
add_library(nlm_performance STATIC
    src/performance/MemoryPool.cpp
    src/performance/CheckpointSystem.cpp
    src/performance/EventQueue.cpp
    src/performance/OptimizedBrain.cpp
    src/performance/SIMDVectorization.cpp
    src/performance/ParallelProcessing.cpp
    src/performance/SparseConnectivity.cpp
)

target_include_directories(nlm_performance PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_performance PUBLIC nlm_core)

# Phase 3 experiments (social learning)
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments (cognitive development)
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Main simulation executable
add_executable(nlm
    src/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demonstration executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demonstration executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 5 demonstration executable (if exists)
find_file(PHASE5_DEMO src/experiments/Phase5Demo.cpp)
if(PHASE5_DEMO)
    add_executable(nlm_phase5_demo src/experiments/Phase5Demo.cpp)
    target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Phase 6 integration demonstration executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)

target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing setup
enable_testing()
add_subdirectory(tests)

# Installation targets for package management
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)