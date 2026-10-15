cmake_minimum_required(VERSION 3.16)
project(NLM-Python-Bindings VERSION 0.1.0 LANGUAGES CXX)

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Find pybind11
find_package(pybind11 REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${Python_INCLUDE_DIRS})
include_directories(${pybind11_INCLUDE_DIR})

# Core library sources
set(NLM_CORE_SOURCES
    # Core utilities
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
    src/neuromodulation/Dopamine.cpp
    src/neuromodulation/Curiosity.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/PredictionError.cpp
    
    # Memory
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
    
    # Agent
    src/agent/AgentBrain.cpp
    src/agent/AgentBody.cpp
    src/agent/SensoryPercept.cpp
    
    # World
    src/world/SimpleWorld.cpp
    
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)

# Python bindings sources
if(BUILD_PYTHON_BINDINGS)
    set(PYTHON_BINDINGS_SOURCES
        src/python/bindings.cpp
    )
endif()

# Create NLM core library
add_library(nlm_core ${NLM_CORE_SOURCES})

# Set library properties
target_compile_features(nlm_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(nlm_core PRIVATE /W4)
else()
    target_compile_options(nlm_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Python bindings executable
if(BUILD_PYTHON_BINDINGS)
    add_executable(pynlm_core ${PYTHON_BINDINGS_SOURCES})
    target_link_libraries(pynlm_core PRIVATE nlm_core pybind11::module)
    
    # Set Python extension properties
    set_target_properties(pynlm_core PROPERTIES
        PREFIX ""
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
        OUTPUT_NAME "_pynlm_core"
    )
    
    # Install Python module
    install(TARGETS pynlm_core
        LIBRARY DESTINATION "${Python_SITEARCH}"
        RUNTIME DESTINATION "${Python_SITELIB}"
    )
endif()

# Build configuration options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Logging configuration
option(NLM_ENABLE_LOGGING "Enable logging" ON)
if(NLM_ENABLE_LOGGING)
    target_compile_definitions(nlm_core PRIVATE "NLM_ENABLE_LOGGING")
endif()

# Performance options
set(NLM_ENABLE_SIMD "Enable SIMD optimizations" ON)
if(NLM_ENABLE_SIMD)
    target_compile_definitions(nlm_core PRIVATE "NLM_ENABLE_SIMD")
endif()

# Memory pool configuration
option(NLM_ENABLE_MEMORY_POOL "Enable memory pool" ON)
if(NLM_ENABLE_MEMORY_POOL)
    target_compile_definitions(nlm_core PRIVATE "NLM_ENABLE_MEMORY_POOL")
endif()

# Event queue configuration
option(NLM_ENABLE_EVENT_QUEUE "Enable event queue" ON)
if(NLM_ENABLE_EVENT_QUEUE)
    target_compile_definitions(nlm_core PRIVATE "NLM_ENABLE_EVENT_QUEUE")
endif()

# Tests
set(TEST_SOURCES
    tests/test_types.cpp
    tests/test_config.cpp
    tests/test_random.cpp
    tests/test_clock.cpp
    tests/test_neuron.cpp
    tests/test_synapse.cpp
    tests/test_stdp.cpp
    tests/test_brain.cpp
    tests/test_main.cpp
)

if(BUILD_TESTS)
    add_executable(nlm_tests ${TEST_SOURCES})
    target_link_libraries(nlm_tests PRIVATE nlm_core)
    
    # Add test dependencies
    find_package(GTest REQUIRED)
    target_link_libraries(nlm_tests PRIVATE GTest::gtest GTest::gtest_main)
    
    enable_testing()
    add_test(NAME nlm_tests COMMAND nlm_tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_executable(nlm_examples
        examples/basic_example.cpp
        examples/agent_example.cpp
        examples/learning_example.cpp
    )
    target_link_libraries(nlm_examples PRIVATE nlm_core)
    
    # Add example tests
    enable_testing()
    add_test(NAME basic_example COMMAND nlm_examples "basic")
    add_test(NAME agent_example COMMAND nlm_examples "agent")
    add_test(NAME learning_example COMMAND nlm_examples "learning")
endif()

# Installation
install(TARGETS nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Configure file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
    @ONLY
)

# Export configuration
export(EXPORT NLMTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake"
)

# Summary
message(STATUS "NLM-Python-Bindings Configuration:")
message(STATUS "  Python executable: ${Python_EXECUTABLE}")
message(STATUS "  Python version: ${Python_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Enable logging: ${NLM_ENABLE_LOGGING}")
message(STATUS "  Enable SIMD: ${NLM_ENABLE_SIMD}")
message(STATUS "  Enable memory pool: ${NLM_ENABLE_MEMORY_POOL}")
message(STATUS "  Enable event queue: ${NLM_ENABLE_EVENT_QUEUE}")
