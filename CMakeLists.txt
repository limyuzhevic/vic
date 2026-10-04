cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++26 standard (enable when ready)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# PGO support
option(PGO_BUILD "Enable Profile-Guided Optimization build" OFF)
option(PGO_GENERATE "Generate PGO profile data" OFF)
option(PGO_USE "Use PGO profile data for optimization" OFF)

# Optional feature flags
option(USE_CCLANG_TIDY "Enable clang-tidy static analysis" OFF)
option(USE_CPPCHECK "Enable cppcheck static analysis" OFF)
option(USE_INCLUDE_WHAT_YOU_USE "Enable include-what-you-use header analysis" OFF)
option(USE_VALGRIND "Enable valgrind memory debugging" OFF)
option(USE_ASAN "Enable address sanitizer" OFF)
option(USE_TSAN "Enable thread sanitizer" OFF)
option(USE_PERF "Enable performance profiler (perf)" OFF)
option(USE_CCACHE "Enable ccache for faster compilation" ON)
option(USE_COVERAGE "Enable code coverage instrumentation" OFF)
option(USE_DOXYGEN "Enable documentation generation with doxygen" OFF)
option(USE_TESTS "Enable testing targets" ON)

# Find tools and libraries
include(GNUInstallDirs)

# PGO configuration
if(PGO_BUILD)
    if(PGO_GENERATE)
        add_compile_options(-fprofile-generate)
        message(STATUS "PGO: Generating profile data with -fprofile-generate")
    elseif(PGO_USE)
        add_compile_options(-fprofile-use)
        message(STATUS "PGO: Using profile data with -fprofile-use")
    endif()
elseif(NOT PGO_BUILD)
    message(STATUS "PGO: Disabled (use -DPGO_BUILD=ON -DPGO_GENERATE=ON to generate profile data)")
endif()

# Development tools configuration
if(USE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_FOUND})
        message(STATUS "CCACHE: Enabled for faster compilation")
    else()
        message(WARNING "CCACHE not found, skipping")
    endif()
endif()

if(USE_COVERAGE)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
    message(STATUS "COVERAGE: Code coverage enabled (--coverage)")
endif()

if(USE_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
    message(STATUS "ASAN: Address sanitizer enabled")
endif()

if(USE_TSAN)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
    message(STATUS "TSAN: Thread sanitizer enabled")
endif()

if(USE_VALGRIND)
    find_program(VALGRIND valgrind)
    if(VALGRIND)
        message(STATUS "VALGRIND: valgrind found at ${VALGRIND}")
    else()
        message(WARNING "VALGRIND not found, valgrind tests will be disabled")
    endif()
endif()

if(USE_PERF)
    find_program(PERF perf)
    if(PERF)
        message(STATUS "PERF: perf found at ${PERF}")
    else()
        message(WARNING "PERF not found, perf tests will be disabled")
    endif()
endif()

# Static analysis configuration
if(USE_CCLANG_TIDY)
    find_program(CLANGTIDY clang-tidy)
    if(CLANGTIDY)
        message(STATUS "CLANGTIDY: Found at ${CLANGTIDY}")
    else()
        message(WARNING "CLANGTIDY not found, skipping clang-tidy analysis")
    endif()
endif()

if(USE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
        message(STATUS "CPPCHECK: Found at ${CPPCHECK}")
    else()
        message(WARNING "CPPCHECK not found, skipping cppcheck analysis")
    endif()
endif()

if(USE_INCLUDE_WHAT_YOU_USE)
    find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
    if(INCLUDE_WHAT_YOU_USE)
        message(STATUS "INCLUDE_WHAT_YOU_USE: Found at ${INCLUDE_WHAT_YOU_USE}")
    else()
        message(WARNING "INCLUDE_WHAT_YOU_USE not found, skipping include analysis")
    endif()
endif()

# Doxygen documentation generation
if(USE_DOXYGEN)
    find_program(DOXYGEN doxygen)
    if(DOXYGEN)
        set(DOXYGEN_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src")
        set(DOXYGEN_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/docs")
        add_custom_target(doxygen_docs
            COMMAND ${DOXYGEN} ${DOXYGEN_INPUT} --output-directory ${DOXYGEN_OUTPUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
        message(STATUS "DOXYGEN: Documentation generation enabled")
    else()
        message(WARNING "DOXYGEN not found, documentation generation will be disabled")
    endif()
endif()

# Configure compiler and build type after all options are set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add PGO optimization flags
if(PGO_BUILD)
    add_compile_options(-fprofile-use)
    add_link_options(-fprofile-use)
endif()
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
    src/neuromodulation/Acetylcholine.cpp
    src/neuromodulation/Norepinephrine.cpp
    src/neuromodulation/Serotonin.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
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
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
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
target_link_libraries(nlm_world PUBLIC nlm_core)

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

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
