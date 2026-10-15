// NLM (熙然) - Neural Learning Machine
// Constants header - reorganized with better structure
// 
// This file provides centralized constants for the NLM project,
// organized by functional domain for better maintainability.

#pragma once

#include <cstddef>
#include <string>

namespace nlm {
namespace constants {

// Neuron physics and dynamics constants
struct NeuronConstants {
    // Membrane properties
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;      // nF
    static constexpr float TIME_CONSTANT = 20.0f;           // ms
    static constexpr float LEAK_CONDUCTANCE = 10.0f;         // nS
    
    // Electrical properties
    static constexpr float DEFAULT_MEMBRANE_POTENTIAL = -70.0f;  // mV
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;    // mV
    static constexpr float DEFAULT_THRESHOLD = -55.0f;           // mV
    static constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;     // mV
    static constexpr float DEFAULT_SYNAPSE_CONDUCTANCE = 0.0f;   // nS
    
    // Refractory period
    static constexpr uint32_t MIN_REFACTORY_PERIOD = 2;        // steps
    static constexpr uint32_t MAX_REFACTORY_PERIOD = 10;       // steps
    static constexpr uint32_t DEFAULT_REFACTORY_PERIOD = 5;    // steps
    
    // Adaptation
    static constexpr float ADAPTATION_DECAY_FACTOR = 0.95f;
    static constexpr float ADAPTATION_INCREMENT = 1.0f;
    
    // Safety bounds
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;   // mV
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;     // mV
    
    // Performance optimization
    static constexpr size_t MAX_SPIKE_HISTORY = 100;           // entries
};

// Synapse-related constants
struct SynapseConstants {
    // Weight ranges
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    static constexpr float DEFAULT_WEIGHT = 0.0f;
    
    // STDP parameters
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;           // ms
    
    // Structural plasticity
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
};

// Memory system constants
struct MemoryConstants {
    // Working memory
    static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    static constexpr float DEFAULT_WORKING_MEMORY_DECAY_RATE = 0.01f;
    static constexpr size_t MEMORY_TRACE_MAX_AGE = 1000;
    static constexpr float MEMORY_ACTIVATION_THRESHOLD = 0.1f;
    
    // Episodic memory
    static constexpr size_t DEFAULT_MAX_EPISODES = 1000;
    static constexpr float EPISODE_CONSOLIDATION_THRESHOLD = 0.3f;
    
    // Competition
    static constexpr float MEMORY_COMPETITION_THRESHOLD = 0.3f;
    static constexpr float WINNER_EXCITATION_STRENGTH = 1.5f;
    static constexpr float LOSER_INHIBITION_STRENGTH = 3.0f;
};

// Neuromodulation constants
struct NeuromodulationConstants {
    // Dopamine
    static constexpr float DOPAMINE_DECAY_RATE = 0.1f;
    static constexpr float DOPAMINE_PEAK_LEVEL = 1.0f;
    static constexpr float DOPAMINE_EXCITABILITY_GAIN = 0.5f;
    
    // Curiosity
    static constexpr float CURIOSITY_THRESHOLD = 0.5f;
    static constexpr float NOVELTY_DETECTION_SENSITIVITY = 0.1f;
    
    // Prediction error
    static constexpr float PREDICTION_ERROR_DECAY = 0.05f;
};

// Cognitive constants
struct CognitiveConstants {
    // Planning
    static constexpr size_t DEFAULT_PLANNING_DEPTH = 5;
    
    // Attention
    static constexpr float DEFAULT_INHIBITION_STRENGTH = 0.5f;
    static constexpr float DEFAULT_EXCITATION_STRENGTH = 1.5f;
    static constexpr float DEFAULT_COMPETITION_THRESHOLD = 0.3f;
    
    // Concept formation
    static constexpr float CONCEPT_FORMATION_LEARNING_RATE = 0.01f;
};

// Development constants
struct DevelopmentConstants {
    // Developmental stages
    static constexpr float PLASTICITY_INITIAL = 1.0f;      // High plasticity
    static constexpr float PLASTICITY_CRITICAL = 0.8f;
    static constexpr float PLASTICITY_MATURE = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;        // Low plasticity
    
    // Timing
    static constexpr size_t CRITICAL_PERIOD_MULTIPLIER = 2;
    static constexpr size_t MATURATION_MULTIPLIER = 4;
    static constexpr size_t ADULT_STABILITY_MULTIPLIER = 10;
};

// Simulation constants
struct SimulationConstants {
    // Time management
    static constexpr double DEFAULT_TIMESTEP = 0.001;     // seconds
    static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;   // steps
    static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // steps
    static constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000; // steps
    static constexpr size_t STRUCTURAL_PLACEMENT_INTERVAL = 100;   // steps
    
    // Input/output scaling
    static constexpr float SENSORY_INPUT_SCALE = 10.0f;     // mV
    static constexpr float SENSORY_INPUT_NORMALIZATION = 1.0f / 10.0f;
    
    // Statistics
    static constexpr size_t STAT_HISTORY_SIZE = 100;
    static constexpr double STAT_COLLECTION_INTERVAL = 0.1; // seconds
};

// Configuration constants
struct ConfigConstants {
    // File paths
    static constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    static constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";
    
    // Defaults
    static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    static constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
    static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    static constexpr size_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    
    // Checkpoints
    static constexpr size_t DEFAULT_CHECKPOINT_INTERVAL = 10000;
    static constexpr size_t DEFAULT_MAX_CHECKPOINTS = 5;
};

// Error handling constants
struct ErrorConstants {
    // Status codes
    static constexpr int SUCCESS = 0;
    static constexpr int FAILURE = 1;
    static constexpr int INVALID_ARGUMENT = 2;
    static constexpr int OUT_OF_MEMORY = 3;
    static constexpr int FILE_NOT_FOUND = 4;
    
    // Tolerance
    static constexpr float FLOAT_EQUALITY_EPSILON = 0.001f;
    static constexpr double DOUBLE_EQUALITY_EPSILON = 1e-9;
    static constexpr float WEIGHT_DELTA_THRESHOLD = 0.01f;
};

// Type aliases for common constants
using NeuronCapacitance = decltype(nlm::constants::NeuronConstants::MEMBRANE_CAPACITANCE);
using TimeConstant = decltype(nlm::constants::NeuronConstants::TIME_CONSTANT);
using LeakConductance = decltype(nlm::constants::NeuronConstants::LEAK_CONDUCTANCE);
using Threshold = decltype(nlm::constants::NeuronConstants::DEFAULT_THRESHOLD);
using Weight = decltype(nlm::constants::SynapseConstants::DEFAULT_WEIGHT);

} // namespace constants

// Backward compatibility namespace
namespace legacy {
    // Old constant names for backward compatibility
    using MEMBRANE_CAPACITANCE = nlm::constants::NeuronConstants::MEMBRANE_CAPACITANCE;
    using TIME_CONSTANT = nlm::constants::NeuronConstants::TIME_CONSTANT;
    using DEFAULT_THRESHOLD = nlm::constants::NeuronConstants::DEFAULT_THRESHOLD;
    using DEFAULT_STDP_LTP_WEIGHT = nlm::constants::SynapseConstants::DEFAULT_STDP_LTP_WEIGHT;
    using DEFAULT_SYNAPTOGENESIS_RATE = nlm::constants::SynapseConstants::DEFAULT_SYNAPTOGENESIS_RATE;
    using DEFAULT_NEURON_COUNT = nlm::constants::ConfigConstants::DEFAULT_NEURON_COUNT;
}

} // namespace nlm
