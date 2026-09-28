# Modular build functions for NLM
# This file contains reusable CMake functions for organizing the build process

function(build_core_libraries)
    message(STATUS "Building core libraries...")
    
    # Core system components
    add_library(nlm_types STATIC
        src/core/Types/Types.cpp
        src/core/Types/Ids.cpp
    )
    target_include_directories(nlm_types PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core/Types>
        $<INSTALL_INTERFACE:include/core/Types>
    )
    
    add_library(nlm_config STATIC
        src/core/Config/Config.cpp
        src/core/Config/ConfigLoader.cpp
    )
    target_include_directories(nlm_config PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core/Config>
        $<INSTALL_INTERFACE:include/core/Config>
    )
    target_link_libraries(nlm_config PUBLIC nlm_types)
    
    add_library(nlm_random STATIC
        src/core/Random/Random.cpp
    )
    target_include_directories(nlm_random PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core/Random>
        $<INSTALL_INTERFACE:include/core/Random>
    )
    
    add_library(nlm_logger STATIC
        src/core/Logger/Logger.cpp
        src/core/Logger/ConsoleLogger.cpp
    )
    target_include_directories(nlm_logger PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core/Logger>
        $<INSTALL_INTERFACE:include/core/Logger>
    )
    
    add_library(nlm_clock STATIC
        src/core/SimulationClock/SimulationClock.cpp
    )
    target_include_directories(nlm_clock PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/core/SimulationClock>
        $<INSTALL_INTERFACE:include/core/SimulationClock>
    )
    
    # Brain components
    add_library(nlm_neuron STATIC
        src/brain/Neuron.cpp
    )
    target_include_directories(nlm_neuron PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/brain>
        $<INSTALL_INTERFACE:include/brain>
    )
    
    add_library(nlm_synapse STATIC
        src/brain/Synapse.cpp
    )
    target_include_directories(nlm_synapse PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/brain>
        $<INSTALL_INTERFACE:include/brain>
    )
    target_link_libraries(nlm_synapse PUBLIC nlm_neuron)
    
    add_library(nlm_neural_population STATIC
        src/brain/NeuralPopulation.cpp
    )
    target_include_directories(nlm_neural_population PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/brain>
        $<INSTALL_INTERFACE:include/brain>
    )
    
    add_library(nlm_neural_region STATIC
        src/brain/NeuralRegion.cpp
    )
    target_include_directories(nlm_neural_region PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/brain>
        $<INSTALL_INTERFACE:include/brain>
    )
    target_link_libraries(nlm_neural_region PUBLIC nlm_neuron nlm_synapse nlm_neural_population)
    
    add_library(nlm_brain STATIC
        src/brain/Brain.cpp
    )
    target_include_directories(nlm_brain PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/brain>
        $<INSTALL_INTERFACE:include/brain>
    )
    target_link_libraries(nlm_brain PUBLIC nlm_neural_region nlm_types nlm_config nlm_random nlm_logger nlm_clock)
    
    # Dynamics components
    add_library(nlm_neural_dynamics STATIC
        src/dynamics/NeuralDynamics.cpp
    )
    target_include_directories(nlm_neural_dynamics PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/dynamics>
        $<INSTALL_INTERFACE:include/dynamics>
    )
    
    add_library(nlm_spike_system STATIC
        src/dynamics/SpikeSystem.cpp
    )
    target_include_directories(nlm_spike_system PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/dynamics>
        $<INSTALL_INTERFACE:include/dynamics>
    )
    target_link_libraries(nlm_spike_system PUBLIC nlm_neural_dynamics nlm_neuron)
    
    # Combine into unified core library
    add_library(nlm_core INTERFACE)
    target_link_libraries(nlm_core INTERFACE
        nlm_brain
        nlm_spike_system
        nlm_neural_dynamics
        nlm_types
        nlm_config
        nlm_random
        nlm_logger
        nlm_clock
        nlm_neural_region
        nlm_synapse
        nlm_neural_population
        nlm_neuron
    )
    
    message(STATUS "  Built ${nlm_core_SOURCES_COUNT} source files into core library")
endfunction()

function(build_agent_system)
    message(STATUS "Building agent system...")
    
    add_library(nlm_agent STATIC
        src/agent/AgentBrain.cpp
        src/agent/AgentBody.cpp
        src/agent/SensoryPercept.cpp
    )
    target_include_directories(nlm_agent PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/agent>
        $<INSTALL_INTERFACE:include/agent>
    )
    target_link_libraries(nlm_agent PUBLIC nlm_core)
    
    message(STATUS "  Agent system ready")
endfunction()

function(build_world_system)
    message(STATUS "Building world system...")
    
    add_library(nlm_world STATIC
        src/world/SimpleWorld.cpp
        src/world/World.cpp
        src/world/Object.cpp
    )
    target_include_directories(nlm_world PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/world>
        $<INSTALL_INTERFACE:include/world>
    )
    target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)
    
    message(STATUS "  World system ready")
endfunction()

function(build_phase3_library)
    message(STATUS "Building Phase 3 experiment library...")
    
    add_library(nlm_phase3 STATIC
        src/experiments/Phase3Experiment.cpp
        src/experiments/Phase3Demo.cpp
    )
    target_include_directories(nlm_phase3 PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/experiments>
        $<INSTALL_INTERFACE:include/experiments>
    )
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
    
    message(STATUS "  Phase 3 library ready")
endfunction()

function(build_phase4_library)
    message(STATUS "Building Phase 4 experiment library...")
    
    add_library(nlm_phase4 STATIC
        src/experiments/Phase4Experiment.cpp
        src/experiments/Phase4Demo.cpp
    )
    target_include_directories(nlm_phase4 PUBLIC
        $<BUILD_INTERFACE:${NLM_SOURCE_DIR}/experiments>
        $<INSTALL_INTERFACE:include/experiments>
    )
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    
    message(STATUS "  Phase 4 library ready")
endfunction()

function(build_main_executable)
    message(STATUS "Building main executable...")
    
    add_executable(nlm
        src/main.cpp
    )
    target_link_libraries(nlm PUBLIC nlm_core)
    
    message(STATUS "  Main executable ready")
endfunction()

function(build_phase3_demo)
    message(STATUS "Building Phase 3 demo executable...")
    
    add_executable(nlm_phase3_demo
        src/experiments/Phase3Demo.cpp
    )
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    
    message(STATUS "  Phase 3 demo ready")
endfunction()

function(build_phase4_demo)
    message(STATUS "Building Phase 4 demo executable...")
    
    add_executable(nlm_phase4_demo
        src/experiments/Phase4Demo.cpp
    )
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    
    message(STATUS "  Phase 4 demo ready")
endfunction()

function(build_python_bindings)
    message(STATUS "Building Python bindings...")
    
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    find_package(pybind11 REQUIRED)
    
    pybind11_add_module(pynlm
        python/bindings.cpp
        python/PythonBrain.cpp
        python/PythonConfig.cpp
        python/PythonWorld.cpp
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE ${Python3_INCLUDE_DIRS})
    
    # Set Python module properties
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pynlm
    )
    
    message(STATUS "  Python bindings ready")
endfunction()

function(install_targets)
    message(STATUS "Configuring installation...")
    
    # Install libraries
    install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
    
    # Install headers
    install(DIRECTORY src/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
        PATTERN "*.h"
    )
    
    # Install executables
    install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
        RUNTIME DESTINATION bin
    )
    
    # Install Python module
    if(NLM_ENABLE_PYTHON_BINDINGS)
        install(TARGETS pynlm
            LIBRARY DESTINATION lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
        )
    end()
    
    message(STATUS "  Installation configured")
endfunction()
