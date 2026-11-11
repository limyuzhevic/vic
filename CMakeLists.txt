cmake_minimum_required(VERSION 3.16)

project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "An Experimental Artificial Developmental Brain"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Configure build configurations for different build types
foreach(CONF IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER ${CONF} CONF_UPPER)
    set(CMAKE_CXX_FLAGS_${CONF_UPPER} "${CMAKE_CXX_FLAGS_${CONF_UPPER}} -O${CMAKE_BUILD_TYPE_OPTIMIZATION_${CONF_UPPER}}")
endforeach()

# Compiler warnings configuration
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Source directory configuration
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE INTERNAL "NLM source directory")

# Forward declarations for circular dependencies
include(CMakeDependentOption)

# Core library with modern CMake practices
add_library(nlm_core STATIC)

# Core sources organized by category
set(NLM_CORE_SOURCES
    # Core utilities
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    
    # Brain components
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    
    # Neural dynamics
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    
    # Plasticity rules
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    
    # Development system
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    
    # Neuromodulation
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    
    # Memory systems
    ${NLM_SOURCE_DIR}/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/performance/CheckpointSystem.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    
    # Cognitive functions
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    
    # Prediction systems
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    
    # Motor functions
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/motor/Action.cpp
    
    # Environment interface
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    
    # Experiment framework
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    
    # Visualization
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
    ${NLM_SOURCE_DIR}/visualization/AdvancedVisualizationInterface.cpp
)

target_sources(nlm_core PRIVATE ${NLM_CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Compiler-specific options
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror -fPIC>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Position-independent code for static library
set_target_properties(nlm_core PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

# Agent system with better dependency management
add_library(nlm_agent STATIC)
target_sources(nlm_agent PRIVATE
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)
set_target_properties(nlm_agent PROPERTIES CXX_STANDARD 20)

# World system with better abstraction
add_library(nlm_world STATIC)
target_sources(nlm_world PRIVATE ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp)
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)
set_target_properties(nlm_world PROPERTIES CXX_STANDARD 20)

# Phase 3 experiments (world interaction)
add_library(nlm_phase3 STATIC)
target_sources(nlm_phase3 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp)
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
set_target_properties(nlm_phase3 PROPERTIES CXX_STANDARD 20)

# Phase 4 experiments (cognitive functions)
add_library(nlm_phase4 STATIC)
target_sources(nlm_phase4 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp)
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
set_target_properties(nlm_phase4 PROPERTIES CXX_STANDARD 20)

# Phase 5 experiments (performance optimizations)
add_library(nlm_phase5 STATIC)
target_sources(nlm_phase5 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase5Experiment.cpp)
target_include_directories(nlm_phase5 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
set_target_properties(nlm_phase5 PROPERTIES CXX_STANDARD 20)

# Phase 6 experiments (integration tests)
add_library(nlm_phase6 STATIC)
target_sources(nlm_phase6 PRIVATE ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp)
target_include_directories(nlm_phase6 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)
set_target_properties(nlm_phase6 PROPERTIES CXX_STANDARD 20)

# Main executable with proper metadata
add_executable(nlm ${NLM_SOURCE_DIR}/main.cpp)
target_link_libraries(nlm PRIVATE nlm_core)
set_target_properties(nlm PROPERTIES
    CXX_STANDARD 20
    OUTPUT_NAME "nlm"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Demo executables with phase-specific configurations
function(add_nlm_demo TARGET_NAME SOURCE_FILE DESCRIPTION)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE nlm_core nlm_agent nlm_world)
    set_target_properties(${TARGET_NAME} PROPERTIES
        CXX_STANDARD 20
        OUTPUT_NAME ${TARGET_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    add_custom_target(${TARGET_NAME}_info COMMENT "Demo: ${DESCRIPTION}")
endfunction()

add_nlm_demo(nlm_phase3_demo ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp "Phase 3: World Interaction")
add_nlm_demo(nlm_phase4_demo ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp "Phase 4: Cognitive Functions")
add_nlm_demo(nlm_phase5_demo ${NLM_SOURCE_DIR}/experiments/Phase5Demo.cpp "Phase 5: Performance Optimizations")
add_nlm_demo(nlm_phase6_demo ${NLM_SOURCE_DIR}/experiments/Phase6Demo.cpp "Phase 6: Integration Tests")
add_nlm_demo(nlm_phase6_text_demo ${NLM_SOURCE_DIR}/experiments/Phase6TextDemo.cpp "Phase 6 Text: Advanced Learning")

# Benchmark executable
add_executable(nlm_benchmark ${NLM_SOURCE_DIR}/benchmark.cpp)
target_link_libraries(nlm_benchmark PRIVATE nlm_core)
set_target_properties(nlm_benchmark PROPERTIES
    CXX_STANDARD 20
    OUTPUT_NAME "nlm_benchmark"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Python bindings with scikit-build-core integration
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    # Configure Python module properties
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
    )
    
    # Python module with enhanced configuration
    pybind11_add_module(pynlm ${CMAKE_CURRENT_SOURCE_DIR}/python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})
    
    # Python-specific compiler options
    target_compile_options(pynlm PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -fPIC>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    )
    
    if(MSVC)
        target_compile_options(pynlm PRIVATE /permissive-)
    endif()
    
    # Installation for Python package
    install(TARGETS pynlm
        LIBRARY DESTINATION python/pynlm
        ARCHIVE DESTINATION python/pynlm
        RUNTIME DESTINATION python/pynlm
    )
endif()

# Testing framework configuration
enable_testing()

# Add tests subdirectory with modern CMake
set(TEST_SOURCE_DIR ${NLM_SOURCE_DIR}/tests)
if(EXISTS ${TEST_SOURCE_DIR})
    add_subdirectory(tests)
endif()

# Installation configuration
install(DIRECTORY ${NLM_SOURCE_DIR}/core/ DESTINATION include/nlm/core
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/brain/ DESTINATION include/nlm/brain
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY ${NLM_SOURCE_DIR}/memory/ DESTINATION include/nlm/memory
    FILES_MATCHING PATTERN "*.hpp"
)

# Configure installation paths
include(GNUInstallDirs)

install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/easy_usage.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# Generate and install configuration for find_package
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE nlm::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)

# Summary output for build configuration
message(STATUS "NLM Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Core library: nlm_core")
message(STATUS "  Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
