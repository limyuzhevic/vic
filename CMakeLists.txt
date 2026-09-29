cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type with options
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Enable warnings with platform detection
if(MSVC)
    add_compile_options(/W4)
    add_definitions(-DNLM_PLATFORM_WINDOWS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)
    add_definitions(-DNLM_PLATFORM_UNIX)
endif()

# Source and build directories
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Configure project-wide options
option(NLM_BUILD_TESTS "Build tests" ON)
option(NLM_BUILD_PYTHON "Build Python bindings" ON)
option(NLM_BUILD_DOCS "Build documentation" OFF)
option(NLM_ENABLE_PERF_MEASURES "Enable performance measurements" OFF)
option(NLM_ENABLE_LOGGING "Enable detailed logging" ON)

# Set standard include directories
include_directories(${NLM_SOURCE_DIR})
include_directories(${NLM_BINARY_DIR})

# Function to create library with proper configuration
function(add_nlm_library target_name sources)
    add_library(${target_name} STATIC ${sources})
    target_include_directories(${target_name} PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    
    # Apply platform-specific configurations
    if(MSVC)
        target_compile_definitions(${target_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_definitions(${target_name} PRIVATE _POSIX_C_SOURCE=200809L)
    endif()
    
    # Add performance optimization for release builds
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target_name} PRIVATE -O3 -march=native)
    endif()
endfunction()

# Function to create executable with proper configuration
function(add_nlm_executable target_name sources)
    add_executable(${target_name} ${sources})
    target_include_directories(${target_name} PRIVATE ${NLM_SOURCE_DIR})
    
    if(NLM_ENABLE_PERF_MEASURES)
        target_compile_definitions(${target_name} PRIVATE NLM_PERF_MEASURES)
    endif()
    
    if(NLM_ENABLE_LOGGING)
        target_compile_definitions(${target_name} PRIVATE NLM_LOGGING)
    endif()
endfunction()

# Core library with organized structure
add_nlm_library(nlm_core
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
    
    # Cognitive mechanisms
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction system
    src/prediction/NeuralPrediction.cpp
    src/prediction/PredictionSystem.cpp
    
    # Motor system
    src/motor/MotorSystem.cpp
    
    # Environment interface
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

# Agent system (brain-world interface)
add_nlm_library(nlm_agent
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, simulation)
add_nlm_library(nlm_world
    src/world/SimpleWorld.cpp
)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experimental system
add_nlm_library(nlm_phase3
    src/experiments/Phase3Experiment.cpp
)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experimental system
add_nlm_library(nlm_phase4
    src/experiments/Phase4Experiment.cpp
)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable - flexible command-line interface
add_nlm_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demonstration executable
add_nlm_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demonstration executable
add_nlm_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration demo executable
add_nlm_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Python bindings (if enabled)
if(NLM_BUILD_PYTHON)
    add_subdirectory(python)
endif()

# Testing framework
if(NLM_BUILD_TESTS)
    enable_testing()
    
    # Add comprehensive test suite
    add_executable(test_all tests/test_all.cpp)
    target_link_libraries(test_all PUBLIC nlm_core)
    add_test(NAME all_tests COMMAND test_all)
    
    # Add individual test executables for parallel testing
    add_executable(test_clock tests/test_clock.cpp)
    target_link_libraries(test_clock PUBLIC nlm_core)
    add_test(NAME test_clock COMMAND test_clock)
    
    add_executable(test_neuron tests/test_neuron.cpp)
    target_link_libraries(test_neuron PUBLIC nlm_core)
    add_test(NAME test_neuron COMMAND test_neuron)
    
    add_executable(test_brain tests/test_brain.cpp)
    target_link_libraries(test_brain PUBLIC nlm_core nlm_agent nlm_world)
    add_test(NAME test_brain COMMAND test_brain)
    
    # Add Python binding tests if enabled
    if(NLM_BUILD_PYTHON)
        # Python tests would go here when pybind11 is available
        # add_test(NAME python_bindings COMMAND python -m pytest tests/)
    endif()
endif()

# Installation configuration
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Generate package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${NLM_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install configuration files
install(FILES
    "${NLM_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)

# Documentation targets
if(NLM_BUILD_DOCS)
    find_program(SPHINX_BUILD sphinx-build)
    if(SPHINX_BUILD)
        add_custom_target(docs
            COMMAND ${SPHINX_BUILD} -b html docs "${CMAKE_CURRENT_BINARY_DIR}/docs/html"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Building documentation"
            VERBATIM
        )
    endif()
endif()

# Summary
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Project Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Tests: ${NLM_BUILD_TESTS}")
message(STATUS "  Python Bindings: ${NLM_BUILD_PYTHON}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")