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

# Find existing source files
file(GLOB_RECURSE NLM_CORE_SOURCES
    "${NLM_SOURCE_DIR}/core/Config/Config.cpp"
    "${NLM_SOURCE_DIR}/core/Logger/Logger.cpp"
    "${NLM_SOURCE_DIR}/core/Random/Random.cpp"
    "${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp"
    "${NLM_SOURCE_DIR}/core/Types/Types.cpp"
    "${NLM_SOURCE_DIR}/brain/Brain.cpp"
    "${NLM_SOURCE_DIR}/brain/Neuron.cpp"
    "${NLM_SOURCE_DIR}/brain/Synapse.cpp"
    "${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp"
    "${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp"
    "${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp"
    "${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp"
    "${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp"
    "${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp"
    "${NLM_SOURCE_DIR}/plasticity/STDP.cpp"
    "${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp"
    "${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp"
    "${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp"
    "${NLM_SOURCE_DIR}/development/Pruning.cpp"
    "${NLM_SOURCE_DIR}/development/Maturation.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp"
    "${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp"
    "${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp"
    "${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp"
    "${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp"
    "${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp"
    "${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp"
    "${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp"
    "${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp"
    "${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp"
    "${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp"
    "${NLM_SOURCE_DIR}/sensory/Vision.cpp"
    "${NLM_SOURCE_DIR}/sensory/Audio.cpp"
    "${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp"
    "${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp"
    "${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp"
    "${NLM_SOURCE_DIR}/motor/MotorSystem.cpp"
    "${NLM_SOURCE_DIR}/environment/Environment.cpp"
    "${NLM_SOURCE_DIR}/environment/Observation.cpp"
    "${NLM_SOURCE_DIR}/environment/Action.cpp"
    "${NLM_SOURCE_DIR}/experiments/Experiment.cpp"
    "${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp"
    "${NLM_SOURCE_DIR}/experiments/Metrics.cpp"
    "${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp"
)

# Agent system (body, sensory percept, agent-brain interface)
file(GLOB_RECURSE NLM_AGENT_SOURCES
    "${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp"
    "${NLM_SOURCE_DIR}/agent/AgentBrain.cpp"
)

# World system (environment, objects, simple world)
file(GLOB_RECURSE NLM_WORLD_SOURCES
    "${NLM_SOURCE_DIR}/world/SimpleWorld.cpp"
)

# Phase 3 experiments
file(GLOB_RECURSE NLM_PHASE3_SOURCES
    "${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp"
)

# Phase 4 experiments
file(GLOB_RECURSE NLM_PHASE4_SOURCES
    "${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp"
)

# Extensible architecture plugins
file(GLOB_RECURSE NLM_PLUGINS_SOURCES
    "${NLM_SOURCE_DIR}/plugins/PluginManager.cpp"
    "${NLM_SOURCE_DIR}/plugins/NeuronModelPlugin.cpp"
    "${NLM_SOURCE_DIR}/plugins/PlasticityRulePlugin.cpp"
    "${NLM_SOURCE_DIR}/plugins/SynapticDynamicsPlugin.cpp"
)

# Advanced API
file(GLOB_RECURSE NLM_ADVANCED_API_SOURCES
    "${NLM_SOURCE_DIR}/core/ExtensibleBrain.cpp"
    "${NLM_SOURCE_DIR}/core/CustomMemorySystem.cpp"
    "${NLM_SOURCE_DIR}/core/AdvancedNeuromodulator.cpp"
    "${NLM_SOURCE_DIR}/arch/Architecture.cpp"
)

# Configuration system
file(GLOB_RECURSE NLM_CONFIG_SOURCES
    "${NLM_SOURCE_DIR}/config/AdvancedConfig.cpp"
    "${NLM_SOURCE_DIR}/config/ConfigValidator.cpp"
    "${NLM_SOURCE_DIR}/config/ConfigInheritance.cpp"
)

# Architecture definition
file(GLOB_RECURSE NLM_ARCH_SOURCES
    "${NLM_SOURCE_DIR}/arch/ArchitectureCompiler.cpp"
    "${NLM_SOURCE_DIR}/arch/ArchitectureParser.cpp"
)

# Optimization hooks
file(GLOB_RECURSE NLM_OPTIM_SOURCES
    "${NLM_SOURCE_DIR}/optim/OptimizationHooks.cpp"
    "${NLM_SOURCE_DIR}/optim/CustomParallelism.cpp"
    "${NLM_SOURCE_DIR}/optim/CustomMemoryManager.cpp"
)

# Experiment framework
file(GLOB_RECURSE NLM_EXPERIMENT_SOURCES
    "${NLM_SOURCE_DIR}/experiment/AdvancedExperiment.cpp"
    "${NLM_SOURCE_DIR}/experiment/HyperparameterOptimizer.cpp"
)

# Main core library
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Plugins library
add_library(nlm_plugins STATIC ${NLM_PLUGINS_SOURCES})
target_include_directories(nlm_plugins PUBLIC src)
target_link_libraries(nlm_plugins PUBLIC nlm_core)

# Advanced API library
add_library(nlm_advanced_api STATIC ${NLM_ADVANCED_API_SOURCES})
target_include_directories(nlm_advanced_api PUBLIC src)
target_link_libraries(nlm_advanced_api PUBLIC nlm_core nlm_plugins)

# Configuration system library
add_library(nlm_config STATIC ${NLM_CONFIG_SOURCES})
target_include_directories(nlm_config PUBLIC src)
target_link_libraries(nlm_config PUBLIC nlm_core)

# Architecture library
add_library(nlm_arch STATIC ${NLM_ARCH_SOURCES})
target_include_directories(nlm_arch PUBLIC src)
target_link_libraries(nlm_arch PUBLIC nlm_core)

# Optimization library
add_library(nlm_optim STATIC ${NLM_OPTIM_SOURCES})
target_include_directories(nlm_optim PUBLIC src)
target_link_libraries(nlm_optim PUBLIC nlm_core)

# Experiment framework library
add_library(nlm_experiment STATIC ${NLM_EXPERIMENT_SOURCES})
target_include_directories(nlm_experiment PUBLIC src)
target_link_libraries(nlm_experiment PUBLIC nlm_core nlm_optim)

# Agent library
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World library
add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC ${NLM_PHASE3_SOURCES})
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC ${NLM_PHASE4_SOURCES})
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PUBLIC nlm_core nlm_advanced_api nlm_config nlm_arch nlm_experiment)

# Phase 3 demo executable
add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
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