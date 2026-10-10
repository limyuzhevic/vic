// CMakeLists.txt - Updated to include new profiling and debugging modules

cmake_minimum_required(VERSION 3.10)
project(NLM VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Source files
set(NLM_SOURCES
    src/main.cpp
    src/core/Logger/Logger.cpp
    src/core/Logger/Logger.hpp
    src/core/EnhancedLogger.cpp
    src/core/EnhancedLogger.hpp
    src/profiling/ProfilingTimer.cpp
    src/profiling/ProfilingTimer.hpp
    src/profiling/NLMProfiling.cpp
    src/profiling/NLMProfilingOutputFormatter.hpp
    src/debug/SystemMonitor.cpp
    src/debug/SystemMonitor.hpp
    src/debug/LoggingEnhancement.cpp
    src/debug/LoggingEnhancement.hpp
    src/benchmark.cpp
)

# Brain components
list(APPEND NLM_SOURCES
    src/brain/Brain.cpp
    src/brain/Neuron.cpp
    src/brain/Neuron.hpp
    src/brain/NeuralRegion.cpp
    src/brain/NeuralRegion.hpp
    src/brain/Synapse.cpp
    src/brain/Synapse.hpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralPopulation.hpp
)

# Core components
list(APPEND NLM_SOURCES
    src/core/Types/Types.cpp
    src/core/Types/Types.hpp
    src/core/Config/Config.cpp
    src/core/Config/Config.hpp
    src/core/Random/Random.cpp
    src/core/Random/Random.hpp
    src/core/SimulationClock/SimulationClock.cpp
    src/core/SimulationClock/SimulationClock.hpp
)

# Neuromodulation components
list(APPEND NLM_SOURCES
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Neuromodulator.hpp
    src/neuromodulation/Dopamine.cpp
    src/neuromodulation/Curiosity.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/PredictionError.cpp
)

# Memory components
list(APPEND NLM_SOURCES
    src/memory/Memory.cpp
    src/memory/Memory.hpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralWorkingMemory.hpp
    src/memory/NeuralEpisodicMemory.cpp
    src/memory/NeuralEpisodicMemory.hpp
    src/memory/NeuralAssociativeMemory.cpp
    src/memory/NeuralAssociativeMemory.hpp
)

# Plasticity components
list(APPEND NLM_SOURCES
    src/plasticity/PlasticityRule.cpp
    src/plasticity/PlasticityRule.hpp
    src/plasticity/STDP.cpp
    src/plasticity/STDP.hpp
    src/plasticity/Hebbian.cpp
    src/plasticity/Hebbian.hpp
    src/plasticity/StructuralPlasticity.cpp
    src/plasticity/StructuralPlasticity.hpp
)

# Development components
list(APPEND NLM_SOURCES
    src/development/DevelopmentSystem.cpp
    src/development/DevelopmentSystem.hpp
    src/development/Synaptogenesis.cpp
    src/development/Synaptogenesis.hpp
    src/development/Maturation.cpp
    src/development/Maturation.hpp
    src/development/Pruning.cpp
    src/development/Pruning.hpp
)

# Prediction components
list(APPEND NLM_SOURCES
    src/prediction/PredictionSystem.cpp
    src/prediction/PredictionSystem.hpp
    src/prediction/NeuralPrediction.cpp
    src/prediction/NeuralPrediction.hpp
)

# Cognition components
list(APPEND NLM_SOURCES
    src/cognition/ConceptFormation.cpp
    src/cognition/ConceptFormation.hpp
    src/cognition/NeuralPlanner.cpp
    src/cognition/NeuralPlanner.hpp
)

# Sensory components
list(APPEND NLM_SOURCES
    src/sensory/SensoryInput.cpp
    src/sensory/SensoryInput.hpp
    src/sensory/Audio.cpp
    src/sensory/Audio.hpp
    src/sensory/Vision.cpp
    src/sensory/Vision.hpp
    src/sensory/InternalSignals.cpp
    src/sensory/InternalSignals.hpp
)

# Motor components
list(APPEND NLM_SOURCES
    src/motor/MotorSystem.cpp
    src/motor/MotorSystem.hpp
    src/motor/Action.cpp
    src/motor/Action.hpp
)

# Environment components
list(APPEND NLM_SOURCES
    src/environment/Environment.cpp
    src/environment/Environment.hpp
    src/environment/Observation.cpp
    src/environment/Observation.hpp
)

# World components
list(APPEND NLM_SOURCES
    src/world/SimpleWorld.cpp
    src/world/SimpleWorld.hpp
    src/world/Observation.cpp
    src/world/Observation.hpp
)

# Dynamics components
list(APPEND NLM_SOURCES
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/NeuralDynamics.hpp
    src/dynamics/SpikeSystem.cpp
    src/dynamics/SpikeSystem.hpp
)

# Performance components
list(APPEND NLM_SOURCES
    src/performance/Performance.hpp
    src/performance/MemoryPool.cpp
    src/performance/MemoryPool.hpp
    src/performance/OptimizedBrain.cpp
    src/performance/OptimizedBrain.hpp
    src/performance/EventQueue.cpp
    src/performance/EventQueue.hpp
    src/performance/ParallelProcessing.cpp
    src/performance/ParallelProcessing.hpp
    src/performance/CheckpointSystem.cpp
    src/performance/CheckpointSystem.hpp
    src/performance/SIMDVectorization.cpp
    src/performance/SIMDVectorization.hpp
    src/performance/SparseConnectivity.cpp
    src/performance/SparseConnectivity.hpp
)

# Experiment components
list(APPEND NLM_SOURCES
    src/experiments/Experiment.cpp
    src/experiments/Experiment.hpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/ExperimentRunner.hpp
    src/experiments/Phase3Experiment.cpp
    src/experiments/Phase3Experiment.hpp
    src/experiments/Phase4Experiment.cpp
    src/experiments/Phase4Experiment.hpp
    src/experiments/Phase4Demo.cpp
    src/experiments/Phase5Experiment.cpp
    src/experiments/Phase5Demo.cpp
    src/experiments/Phase6Demo.cpp
    src/experiments/Phase6IntegratedExperiment.cpp
    src/experiments/Phase6IntegratedExperiment.hpp
    src/experiments/Metrics.cpp
    src/experiments/Metrics.hpp
    src/experiments/ScalingBenchmark.cpp
    src/experiments/ScalingBenchmark.hpp
)

# CLI components
list(APPEND NLM_SOURCES
    src/cli/NLMCLI.cpp
    src/cli/NLMCLI.hpp
)

# Agent components
list(APPEND NLM_SOURCES
    src/agent/AgentBrain.cpp
    src/agent/AgentBrain.hpp
    src/agent/SensoryPercept.cpp
    src/agent/SensoryPercept.hpp
    src/agent/AgentBody.cpp
    src/agent/AgentBody.hpp
)

# Visualization components
list(APPEND NLM_SOURCES
    src/visualization/VisualizationInterface.cpp
    src/visualization/VisualizationInterface.hpp
)

# Build the executable
add_executable(nlm ${NLM_SOURCES})

# Link libraries
target_link_libraries(nlm PUBLIC Threads::Threads)

# Install
install(TARGETS nlm DESTINATION bin)

# Enable profiling and debugging if requested
option(NLM_PROFILING_ENABLED "Enable profiling and debugging features" ON)
if(NLM_PROFILING_ENABLED)
    target_compile_definitions(nlm PUBLIC NLM_PROFILING_ENABLED)
endif()

# Set executable properties
set_target_properties(nlm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    DEBUG_POSTFIX "_d"
)
