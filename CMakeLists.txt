cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard with strict conformance
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # PIC for shared libraries

# Build type options
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not specified, using Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Configuration options
option(NLM_BUILD_TESTS "Build unit tests" ON)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings for pynlm" OFF)
option(NLM_BUILD_DOCS "Build documentation" OFF)
option(NLM_ENABLE_PERFORMANCE_COUNTERS "Enable performance counters" ON)
option(NLM_ENABLE_DEBUG_LOGGING "Enable debug logging by default" OFF)

# Compiler-specific optimizations and warnings
if(MSVC)
    add_compile_options(/W4 /WX)
    add_definitions(-DNLM_DEBUG="$(NLM_ENABLE_DEBUG_LOGGING)")
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type -Werror=uninitialized -Werror=missing-include-dirs)
    # Position-independent code for shared libraries (especially important for Python bindings)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Optimization levels per build type
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG -DNLM_DEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Installation directories
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Installation prefix")
set(INSTALL_LIB_DIR lib)
set(INSTALL_BIN_DIR bin)
set(INSTALL_INCLUDE_DIR include)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CMAKE_SOURCE_INCLUDE_DIR "${NLM_SOURCE_DIR}")

# Function to add source files with consistent patterns
function add_nlm_source_library target_name)
    add_library(${target_name} STATIC ${ARGV})
endfunction()

# Platform-specific dependencies and optimizations
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_options(-pthread -march=native)
    add_link_options(-pthread)
endif()

# Enable performance counters if requested
if(NLM_ENABLE_PERFORMANCE_COUNTERS)
    add_definitions(-DNLM_PERFORMANCE_COUNTERS)
endif()

# Use the add_nlm_source_library function for core library
add_nlm_source_library(nlm_core
    # Core modules
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    
    # Brain modules
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    
    # Neural dynamics
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    
    # Plasticity mechanisms
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    
    # Development processes
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
    
    # Environment interface
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/motor/Action.cpp
    
    # Experiment framework
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    
    # Visualization
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_nlm_source_library(nlm_agent
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${CMAKE_SOURCE_INCLUDE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_nlm_source_library(nlm_world
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC ${CMAKE_SOURCE_INCLUDE_DIR})
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

# Phase 6 text/thought demo (new in Phase 6): text-as-a-sense
# association probe, concept/social probes, and a sandbox run with
# a teacher, planner overrides, and a checkpoint round-trip. The old
# nlm_phase6_demo integration demo is left unchanged.
add_executable(nlm_phase6_text_demo
    src/experiments/Phase6TextDemo.cpp
)
target_link_libraries(nlm_phase6_text_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5 nlm_phase6)

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

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_benchmark
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
