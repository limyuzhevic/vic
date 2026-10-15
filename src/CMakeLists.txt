# Source directory CMakeLists.txt - Modular build system

# Core library
add_library(nlm_core STATIC
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
)

# Brain components
add_library(nlm_brain STATIC
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
)

# Dynamics components
add_library(nlm_dynamics STATIC
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
)

# Plasticity components
add_library(nlm_plasticity STATIC
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
)

# Development components
add_library(nlm_development STATIC
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
)

# Neuromodulation components
add_library(nlm_neuromodulation STATIC
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
)

# Memory components
add_library(nlm_memory STATIC
    ${NLM_SOURCE_DIR}/memory/WorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/EpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/memory/SemanticMemory.cpp
    ${NLM_SOURCE_DIR}/memory/ProceduralMemory.cpp
    ${NLM_SOURCE_DIR}/memory/AssociativeMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
)

# Cognition components
add_library(nlm_cognition STATIC
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
)

# Sensory components
add_library(nlm_sensory STATIC
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
)

# Prediction components
add_library(nlm_prediction STATIC
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
)

# Motor components
add_library(nlm_motor STATIC
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
)

# Environment components
add_library(nlm_environment STATIC
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/environment/Action.cpp
)

# Experiment components
add_library(nlm_experiments STATIC
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
)

# Visualization components
add_library(nlm_visualization STATIC
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Agent system components
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

# World system components
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

# Set include directories for all libraries
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(nlm_brain PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_dynamics PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_plasticity PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_development PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_neuromodulation PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_memory PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_cognition PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_sensory PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_prediction PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_motor PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_environment PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_experiments PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_visualization PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})

# Link dependencies
target_link_libraries(nlm_agent PUBLIC nlm_core)
target_link_libraries(nlm_world PUBLIC nlm_core)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Set compiler options
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Add all libraries to a single alias for easy linking
add_library(NLM_ALL_LIBS INTERFACE)
target_link_libraries(NLM_ALL_LIBS INTERFACE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)