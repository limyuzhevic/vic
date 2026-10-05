# Source directory CMakeLists.txt
# This file implements target-based library organization for NLM project

# Import modern CMake functions
include(cmake/modern_cmake.cmake)

# Configure build type if not already set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# Create core library with target-based approach
nlm_add_library(nlm_core)

# Target sources for core library
target_sources(nlm_core PRIVATE
    # Core components
    core/Types/Types.cpp
    core/Config/Config.cpp
    core/Random/Random.cpp
    core/Logger/Logger.cpp
    core/SimulationClock/SimulationClock.cpp
    core/EnhancedConfig.cpp
    core/ParameterValidator.cpp
    
    # Brain components (integrated Phase 6)
    brain/Neuron.cpp
    brain/Synapse.cpp
    brain/NeuralPopulation.cpp
    brain/NeuralRegion.cpp
    brain/Brain.cpp
    
    # Dynamics
    dynamics/NeuralDynamics.cpp
    dynamics/SpikeSystem.cpp
    
    # Plasticity
    plasticity/PlasticityRule.cpp
    plasticity/Hebbian.cpp
    plasticity/STDP.cpp
    plasticity/StructuralPlasticity.cpp
    
    # Development
    development/DevelopmentSystem.cpp
    development/Synaptogenesis.cpp
    development/Pruning.cpp
    development/Maturation.cpp
    
    # Neuromodulation (Phase 6 integrated)
    neuromodulation/Neuromodulator.cpp
    neuromodulation/Reward.cpp
    neuromodulation/PredictionError.cpp
    neuromodulation/Novelty.cpp
    neuromodulation/Curiosity.cpp
    
    # Memory systems (Phase 6 integrated)
    memory/WorkingMemory.cpp
    memory/EpisodicMemory.cpp
    memory/SemanticMemory.cpp
    memory/ProceduralMemory.cpp
    memory/AssociativeMemory.cpp
    memory/NeuralWorkingMemory.cpp
    memory/NeuralEpisodicMemory.cpp
    memory/Memory.cpp
    
    # Cognition (Phase 6 integrated)
    cognition/ConceptFormation.cpp
    cognition/NeuralPlanner.cpp
    
    # Sensory
    sensory/SensoryInput.cpp
    sensory/Vision.cpp
    sensory/Audio.cpp
    sensory/InternalSignals.cpp
    
    # Prediction (Phase 6 integrated)
    prediction/PredictionSystem.cpp
    prediction/NeuralPrediction.cpp
    
    # Motor
    motor/MotorSystem.cpp
    motor/Action.cpp
    
    # Environment
    environment/Environment.cpp
    environment/Observation.cpp
    environment/Action.cpp
    environment/SimpleWorld.cpp
    world/Observation.cpp
    
    # Experiments (Phase 6 integrated)
    experiments/Experiment.cpp
    experiments/ExperimentRunner.cpp
    experiments/Metrics.cpp
    experiments/Phase3Experiment.cpp
    experiments/Phase4Experiment.cpp
    experiments/Phase5Experiment.cpp
    experiments/Phase6IntegratedExperiment.cpp
    
    # Visualization
    visualization/VisualizationInterface.cpp
    
    # Agent systems
    agent/SensoryPercept.cpp
    agent/AgentBrain.cpp
    agent/AgentBody.cpp
)

# Target include directories for core library
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Apply build type and platform-specific options
apply_build_type_options(nlm_core)
apply_platform_specific_options(nlm_core)
apply_compiler_optimizations(nlm_core)

# Create agent system library
add_library(nlm_agent STATIC)
target_sources(nlm_agent PRIVATE
    agent/SensoryPercept.cpp
    agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# Create world system library
add_library(nlm_world STATIC)
target_sources(nlm_world PRIVATE
    world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Create Phase 3 experiments library
add_library(nlm_phase3 STATIC)
target_sources(nlm_phase3 PRIVATE
    experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Create Phase 4 experiments library
add_library(nlm_phase4 STATIC)
target_sources(nlm_phase4 PRIVATE
    experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
nlm_add_executable(nlm src/main.cpp)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 5 demo executable
add_executable(nlm_phase5_demo src/experiments/Phase5Demo.cpp)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 integration demo (Phase 6 final)
add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Add performance benchmarks
add_executable(nlm_benchmark src/benchmark.cpp)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Python bindings (via pybind11)
# This will be handled by the python/CMakeLists.txt

# Export targets for downstream usage
export(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
       FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake")
export(PACKAGE NLM)

# Add custom targets
add_custom_target(build_info
    COMMAND ${CMAKE_COMMAND} -E echo "Build directory: ${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Build type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
    COMMENT "Display build information"
    VERBATIM
)

# Add installation targets
include(cmake/installation.cmake)
install_targets()
install_headers()
install_runtime_configs()

# Generate configuration files
generate_config_header()