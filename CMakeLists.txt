cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library
add_library(nlm_core STATIC
    # Core
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
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    # Memory
    src/memory/Memory.cpp
    src/performance/CheckpointSystem.cpp
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
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/motor/Action.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 5 demo executable (Phase 5, added here: pattern probe,
# sandbox run, and checkpoint round-trip for the brain-personality
# step — previously Phase 5 was library-only with no demo).
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Benchmark executable
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Python bindings (Phase 5: this is what `pip install .` builds).
# Previously pyproject.toml claimed scikit-build-core would build pynlm,
# but the root CMakeLists never built the module — pip install
# succeeded yet `import pynlm` failed. The option is OFF by default so
# a plain C++ build needs no Python/pybind11; pyproject.toml turns it
# ON. The standalone python/CMakeLists.txt build still works too, but
# it must not be added here as a subdirectory: it defines its own
# nlm_core/nlm_agent/nlm_world targets, which would collide with the
# ones above. This target reuses the root libraries instead.
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module" OFF)
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    # The module is shared, so the static libs it links need PIC.
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE src)
    install(TARGETS pynlm LIBRARY DESTINATION .)
endif()

# Phase 6: Final Integration - NLM Advanced Improvements

# Enable advanced configuration and error handling
add_definitions(-DNLM_ADVANCED_FEATURES)

# Core library with advanced improvements
add_library(nlm_core_advanced STATIC
    # Core utilities with improved error handling
    src/core/Config/Config.cpp
    
    # Brain with enhanced save/load and validation
    src/brain/Brain.cpp
    
    # Agent system improvements
    src/agent/AgentBrain.cpp
    
    # Batch processing and debugging tools
    src/core/BatchProcessor.cpp
    src/core/BatchProcessorBindings.cpp
    
    # Performance monitoring and analysis
    src/performance/CheckpointSystem.cpp
    src/performance/Performance.cpp
    src/performance/MemoryPool.cpp
    
    # Visualization and analysis tools
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core_advanced PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Python bindings for advanced features
set(PYTHON_MODULE_ADVANCED pynlm_advanced)
pybind11_add_module(${PYTHON_MODULE_ADVANCED}
    python/bindings.cpp
    src/core/BatchProcessorBindings.cpp
)
target_link_libraries(${PYTHON_MODULE_ADVANCED} PRIVATE nlm_core_advanced)

# Advanced demo executable for testing new features
add_executable(nlm_advanced_demo
    src/experiments/AdvancedExperiment.cpp
)
target_link_libraries(nlm_advanced_demo PUBLIC nlm_core_advanced)

# Batch processing demo
add_executable(nlm_batch_demo
    src/experiments/BatchProcessingDemo.cpp
)
target_link_libraries(nlm_batch_demo PUBLIC nlm_core_advanced)

# Debugging and analysis demo
add_executable(nlm_debug_demo
    src/experiments/DebuggingDemo.cpp
)
target_link_libraries(nlm_debug_demo PUBLIC nlm_core_advanced)

# Integration test for advanced features
add_executable(nlm_advanced_test
    tests/test_advanced_improvements.cpp
)
target_link_libraries(nlm_advanced_test PUBLIC nlm_core_advanced)

# Install advanced tools
install(TARGETS nlm_advanced_demo nlm_batch_demo nlm_debug_demo nlm_advanced_test
    RUNTIME DESTINATION bin
)

install(TARGETS ${PYTHON_MODULE_ADVANCED}
    LIBRARY DESTINATION lib
)

# Documentation for advanced features
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/ADVANCED_FEATURES.md.in
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/ADVANCED_FEATURES.md
)

# Add test targets for advanced features
enable_testing()
add_test(NAME advanced_features_test
    COMMAND nlm_advanced_test
)

# Add benchmark for advanced features
add_executable(nlm_advanced_benchmark
    src/benchmarks/AdvancedFeaturesBenchmark.cpp
)
target_link_libraries(nlm_advanced_benchmark PUBLIC nlm_core_advanced)
add_test(NAME advanced_benchmark_test
    COMMAND nlm_advanced_benchmark
)

# Summary of improvements for Phase 6
message(STATUS "Phase 6 Advanced Improvements:")
message(STATUS "  ✓ Fixed Config::saveToFile() placeholder values bug")
message(STATUS "  ✓ Added comprehensive error handling and validation")
message(STATUS "  ✓ Enhanced Brain::save() and Brain::load() error recovery")
message(STATUS "  ✓ Added batch processing utilities with progress tracking")
message(STATUS "  ✓ Added advanced debugging and analysis tools")
message(STATUS "  ✓ Added visualization and performance monitoring")
message(STATUS "  ✓ Enhanced Python bindings with new advanced APIs")
message(STATUS "  ✓ Added configuration templates and management tools")
message(STATUS "  ✓ Added comprehensive test coverage")
message(STATUS "  ✓ Improved documentation and examples")
