# CMake improvements for NLM Phase 6

# Python bindings fix (Phase 5.5): The original issue was that pip install succeeded but import pynlm failed
# because the module wasn't properly linked with position-independent code and had incomplete dependencies.
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module" ON)
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    # Ensure all core libraries have position-independent code for shared library usage
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
    
    # Create the Python module with proper dependencies
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE src)
    
    # Set output directory for Python module to match typical installation expectations
    set_target_properties(pynlm PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pynlm"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pynlm"
    )
    
    # Install Python module to appropriate location
    install(TARGETS pynlm
        LIBRARY DESTINATION lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
        RUNTIME DESTINATION bin/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
    )
    
    message(STATUS "Building Python bindings for NLM")
endif()

# Improved build configuration
option(NLM_BUILD_EXAMPLES "Build demo executables" ON)
option(NLM_BUILD_TESTS "Build test suite" ON)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)

if(NLM_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# Better library organization with component-based dependencies
add_library(nlm_core
    # Core utilities
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Neural computation
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Neural dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
)

add_library(nlm_plasticity
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)

add_library(nlm_development
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
)
target_link_libraries(nlm_development PUBLIC nlm_core nlm_plasticity)

add_library(nlm_neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core nlm_plasticity nlm_development)

add_library(nlm_memory
    src/memory/Memory.cpp
    src/performance/CheckpointSystem.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
)
target_link_libraries(nlm_memory PUBLIC nlm_core nlm_plasticity nlm_development nlm_neuromodulation)

add_library(nlm_cognition
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
)
target_link_libraries(nlm_cognition PUBLIC nlm_core nlm_memory nlm_neuromodulation)

add_library(nlm_sensory
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
)
target_link_libraries(nlm_sensory PUBLIC nlm_core)

add_library(nlm_prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)
target_link_libraries(nlm_prediction PUBLIC nlm_core nlm_neuromodulation)

add_library(nlm_motor
    src/motor/MotorSystem.cpp
    src/motor/Action.cpp
)
target_link_libraries(nlm_motor PUBLIC nlm_core)

add_library(nlm_environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
)
target_link_libraries(nlm_environment PUBLIC nlm_core nlm_sensory nlm_motor)

add_library(nlm_agent
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_sensory nlm_neuromodulation nlm_cognition)

add_library(nlm_world
    src/world/SimpleWorld.cpp
)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent nlm_environment)

# Agent system (body, sensory percept, agent-brain interface)

# Phase experiments
add_library(nlm_phase3
    src/experiments/Phase3Experiment.cpp
)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

add_library(nlm_phase4
    src/experiments/Phase4Experiment.cpp
)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

add_library(nlm_phase5
    src/experiments/Phase5Experiment.cpp
)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 6 experiments (Final Integration)
add_library(nlm_phase6
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5 nlm_plasticity nlm_development nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Demo executables (Phase 3)
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Demo executables (Phase 4)
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Demo executables (Phase 5)
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

# Demo executables (Phase 6)
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Demo executables (Phase 6 Text)
add_executable(nlm_phase6_text_demo
    src/experiments/Phase6TextDemo.cpp
)
target_link_libraries(nlm_phase6_text_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5 nlm_phase6)

# Benchmark executable
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Testing framework
if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    
    # Add some standard test targets
    add_test(NAME nlm_phase2_test COMMAND nlm_benchmark)
    add_test(NAME nlm_phase6_demo_test COMMAND nlm_phase6_demo)
endif()

# Installation targets
install(TARGETS nlm nlm_benchmark
    RUNTIME DESTINATION bin
    NAMELINK_COMPONENT development
)

install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    NAMELINK_COMPONENT development
)

# Install header files
install(DIRECTORY src/
    DESTINATION include/nlm
    FILES_MATCHING PATTERN "*.hpp"
    FILES_MATCHING PATTERN "*.h"
)

# Python package installation (for pip)
if(NLM_BUILD_PYTHON_BINDINGS)
    install(FILES python/__init__.py python/bindings.cpp
        DESTINATION lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages/pynlm
    )
endif()

# Package configuration for find_package support
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION lib/cmake/NLM
)

# Summary message
message(STATUS "NLM Phase 6 build configuration complete")
message(STATUS "  Core libraries: nlm_core, nlm_agent, nlm_world")
message(STATUS "  Functional components: nlm_plasticity, nlm_development, nlm_neuromodulation, nlm_memory, nlm_cognition, nlm_sensory, nlm_prediction, nlm_motor, nlm_environment")
message(STATUS "  Phase libraries: nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6")
message(STATUS "  Executables: nlm, nlm_phase*_demo, nlm_benchmark")
if(NLM_BUILD_PYTHON_BINDINGS)
    message(STATUS "  Python bindings: pynlm")
endif()
if(NLM_BUILD_TESTS)
    message(STATUS "  Tests: enabled")
endif()