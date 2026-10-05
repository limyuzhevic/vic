cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 1.0.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Position independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable comprehensive warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
endif()

# Compiler optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Component options for modular building
option(NLM_ENABLE_PHASE2 "Enable Phase 2 neural computation" ON)
option(NLM_ENABLE_PHASE3 "Enable Phase 3 experiments" ON)
option(NLM_ENABLE_PHASE4 "Enable Phase 4 cognitive experiments" ON)
option(NLM_ENABLE_PHASE6 "Enable Phase 6 final integration" ON)
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_CLI "Enable Command Line Interface" ON)
option(NLM_ENABLE_PERFORMANCE_PROFILING "Enable performance profiling" ON)
option(NLM_ENABLE_PLUGINS "Enable plugin system" ON)
option(NLM_ENABLE_VISUALIZATION "Enable visualization components" ON)
option(NLM_ENABLE_EXPERIMENTS "Enable experiment framework" ON)
option(NLM_ENABLE_TESTING "Enable comprehensive testing" ON)
option(NLM_ENABLE_DOCS "Enable documentation generation" ON)

# Automatic source file discovery function
function(find_source_files var base_dir)
    file(GLOB_RECURSE sources ${base_dir}/*.cpp)
    file(GLOB_RECURSE headers ${base_dir}/*.hpp)
    file(GLOB_RECURSE cmake_files ${base_dir}/*.cmake)
    
    list(FILTER sources EXCLUDE "*/tests/*")
    list(FILTER headers EXCLUDE "*/tests/*")
    
    set(${var}_SOURCES ${sources} PARENT_SCOPE)
    set(${var}_HEADERS ${headers} PARENT_SCOPE)
    set(${var}_CMAKE_FILES ${cmake_files} PARENT_SCOPE)
endfunction()

# Find all source files
find_source_files(nlm_core "${NLM_SOURCE_DIR}")

# Core library (Phase 2: Real Neural Computation)
if(NLM_ENABLE_PHASE2)
    add_library(nlm_core STATIC
        ${nlm_core_SOURCES}
        # Core utilities
        src/core/Types/Types.cpp
        src/core/Config/Config.cpp
        src/core/Random/Random.cpp
        src/core/Logger/Logger.cpp
        src/core/SimulationClock/SimulationClock.cpp
        # Brain (Phase 2)
        src/brain/Neuron.cpp
        src/brain/Synapse.cpp
        src/brain/NeuralPopulation.cpp
        src/brain/NeuralRegion.cpp
        src/brain/Brain.cpp
        # Dynamics (Phase 2)
        src/dynamics/NeuralDynamics.cpp
        src/dynamics/SpikeSystem.cpp
        # Plasticity (Phase 2)
        src/plasticity/PlasticityRule.cpp
        src/plasticity/Hebbian.cpp
        src/plasticity/STDP.cpp
        src/plasticity/StructuralPlasticity.cpp
        # Development (Phase 2)
        src/development/DevelopmentSystem.cpp
        src/development/Synaptogenesis.cpp
        src/development/Pruning.cpp
        src/development/Maturation.cpp
        # Neuromodulation (Phase 2)
        src/neuromodulation/Neuromodulator.cpp
        src/neuromodulation/Reward.cpp
        src/neuromodulation/PredictionError.cpp
        src/neuromodulation/Novelty.cpp
        src/neuromodulation/Curiosity.cpp
        # Memory (Phase 2)
        src/memory/WorkingMemory.cpp
        src/memory/EpisodicMemory.cpp
        src/memory/SemanticMemory.cpp
        src/memory/ProceduralMemory.cpp
        src/memory/AssociativeMemory.cpp
        src/memory/NeuralWorkingMemory.cpp
        src/memory/NeuralEpisodicMemory.cpp
        # Cognition (Phase 2)
        src/cognition/ConceptFormation.cpp
        src/cognition/NeuralPlanner.cpp
        # Sensory (Phase 2)
        src/sensory/SensoryInput.cpp
        src/sensory/Vision.cpp
        src/sensory/Audio.cpp
        src/sensory/InternalSignals.cpp
        # Prediction (Phase 2)
        src/prediction/PredictionSystem.cpp
        src/prediction/NeuralPrediction.cpp
        # Motor (Phase 2)
        src/motor/MotorSystem.cpp
        # Environment (Phase 2)
        src/environment/Environment.cpp
        src/environment/Observation.cpp
        src/environment/Action.cpp
    )
    
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    )
    
    target_compile_definitions(nlm_core PUBLIC
        NLM_PHASE2_ENABLED
        NLM_CORE_IMPLEMENTATION
    )
    
    # Performance optimizations for core library
    target_link_libraries(nlm_core PUBLIC
        $<$<BOOL:${NLM_ENABLE_PERFORMANCE_PROFILING}>:profiling>
    )
endif()

# Plugin system library
if(NLM_ENABLE_PLUGINS)
    add_library(nlm_plugins STATIC
        src/plugins/PluginInterface.cpp
        src/plugins/PluginManager.cpp
        src/plugins/PluginLoader.cpp
        src/plugins/PluginBase.cpp
    )
    
    target_include_directories(nlm_plugins PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
    
    target_compile_definitions(nlm_plugins PUBLIC NLM_PLUGIN_SYSTEM)
    
    target_link_libraries(nlm_plugins PUBLIC nlm_core)
endif()

# Agent system (enhanced)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
    src/agent/AgentBody.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (enhanced)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/WorldInterface.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
if(NLM_ENABLE_PHASE3)
    add_library(nlm_phase3 STATIC
        src/experiments/Phase3Experiment.cpp
        src/experiments/Phase3Demo.cpp
    )
    target_include_directories(nlm_phase3 PUBLIC src)
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 4 experiments  
if(NLM_ENABLE_PHASE4)
    add_library(nlm_phase4 STATIC
        src/experiments/Phase4Experiment.cpp
        src/experiments/Phase4Demo.cpp
    )
    target_include_directories(nlm_phase4 PUBLIC src)
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 6 experiments (Final Integration)
if(NLM_ENABLE_PHASE6)
    add_library(nlm_phase6 STATIC
        src/experiments/Phase6Demo.cpp
        src/experiments/Phase6IntegratedExperiment.cpp
    )
    target_include_directories(nlm_phase6 PUBLIC src)
    target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Python bindings
if(NLM_ENABLE_PYTHON_BINDINGS)
    add_library(nlm_py MODULE
        src/python/bindings.cpp
        src/python/PyBind11Module.cpp
        src/python/PyBind11Brain.cpp
        src/python/PyBind11Neuron.cpp
        src/python/PyBind11Synapse.cpp
    )
    
    target_include_directories(nlm_py PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
    
    target_compile_definitions(nlm_py PUBLIC
        PYBIND11_MODULE
        NLM_PYTHON_BINDINGS
    )
    
    # Python-specific settings
    set_target_properties(nlm_py PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        OUTPUT_NAME "nlm_py"
    )
    
    # Link with Python libraries
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    
    target_link_libraries(nlm_py PUBLIC
        nlm_core
        Python3::Python
        Python3::Module
        pybind11::module
    )
endif()

# Command Line Interface
if(NLM_ENABLE_CLI)
    add_executable(nlm_cli
        src/nlm_cli.cpp
    )
    
    target_include_directories(nlm_cli PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
    
    target_link_libraries(nlm_cli PUBLIC
        nlm_core
    )
    
    # Install CLI tool
    install(TARGETS nlm_cli DESTINATION bin)
endif()

# Main executable (Phase 2: Real Neural Computation)
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
if(NLM_ENABLE_PHASE3)
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(NLM_ENABLE_PHASE4)
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Phase 6 demo executable (Final Integration)
if(NLM_ENABLE_PHASE6)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6Demo.cpp
    )
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase6)
    target_compile_definitions(nlm_phase6_demo PUBLIC NLM_PHASE6_DEMO)
endif()

# Performance benchmarks
if(NLM_ENABLE_PERFORMANCE_PROFILING)
    add_executable(benchmarks
        src/benchmark.cpp
    )
    target_link_libraries(benchmarks PUBLIC nlm_core)
endif()

# Testing framework
if(NLM_ENABLE_TESTING)
    enable_testing()
    
    # Unit tests
    add_executable(test_types
        tests/test_types.cpp
    )
    target_link_libraries(test_types PUBLIC nlm_core)
    add_test(NAME test_types COMMAND test_types)
    
    add_executable(test_config
        tests/test_config.cpp
    )
    target_link_libraries(test_config PUBLIC nlm_core)
    add_test(NAME test_config COMMAND test_config)
    
    add_executable(test_neuron
        tests/test_neuron.cpp
    )
    target_link_libraries(test_neuron PUBLIC nlm_core)
    add_test(NAME test_neuron COMMAND test_neuron)
    
    add_executable(test_synapse
        tests/test_synapse.cpp
    )
    target_link_libraries(test_synapse PUBLIC nlm_core)
    add_test(NAME test_synapse COMMAND test_synapse)
    
    add_executable(test_stdp
        tests/test_stdp.cpp
    )
    target_link_libraries(test_stdp PUBLIC nlm_core)
    add_test(NAME test_stdp COMMAND test_stdp)
    
    add_executable(test_brain
        tests/test_brain.cpp
    )
    target_link_libraries(test_brain PUBLIC nlm_core)
    add_test(NAME test_brain COMMAND test_brain)
    
    # Integration tests
    if(NLM_ENABLE_PHASE6)
        add_test(NAME phase6_integration
            COMMAND nlm_phase6_demo
        )
    endif()
endif()

# Documentation generation
if(NLM_ENABLE_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# Install targets
install(TARGETS nlm nlm_core nlm_agent nlm_world
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

if(NLM_ENABLE_PHASE3)
    install(TARGETS nlm_phase3 DESTINATION lib)
endif()

if(NLM_ENABLE_PHASE4)
    install(TARGETS nlm_phase4 DESTINATION lib)
endif()

if(NLM_ENABLE_PHASE6)
    install(TARGETS nlm_phase6 DESTINATION lib)
endif()

if(NLM_ENABLE_PYTHON_BINDINGS)
    install(TARGETS nlm_py DESTINATION lib/python3.12/site-packages)
endif()

if(NLM_ENABLE_CLI)
    install(TARGETS nlm_cli DESTINATION bin)
endif()

# Documentation and configuration files
install(DIRECTORY configs/ DESTINATION include/nlm/config FILES_MATCHING "*.cfg")
install(FILES README.md docs/ ARCHITECTURE.md DESTINATION doc)

# Summary message
message(STATUS "NLM Build Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Phase 2 (Core): ${NLM_ENABLE_PHASE2}")
message(STATUS "  Phase 3 (Experiments): ${NLM_ENABLE_PHASE3}")
message(STATUS "  Phase 4 (Cognition): ${NLM_ENABLE_PHASE4}")
message(STATUS "  Phase 6 (Final Integration): ${NLM_ENABLE_PHASE6}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  CLI tool: ${NLM_ENABLE_CLI}")
message(STATUS "  Performance profiling: ${NLM_ENABLE_PERFORMANCE_PROFILING}")
message(STATUS "  Plugin system: ${NLM_ENABLE_PLUGINS}")
message(STATUS "  Visualization: ${NLM_ENABLE_VISUALIZATION}")
message(STATUS "  Experiments: ${NLM_ENABLE_EXPERIMENTS}")
message(STATUS "  Testing: ${NLM_ENABLE_TESTING}")
message(STATUS "  Documentation: ${NLM_ENABLE_DOCS}")