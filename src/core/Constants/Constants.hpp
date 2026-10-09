// Constants for NLM brain simulation
// Centralized configuration values to improve maintainability and reduce magic numbers

#pragma once

namespace nlm {

// Neural simulation constants
namespace NeuralConstants {
    // Time and timestep
    constexpr TimestepDuration DEFAULT_TIMESTEP = 0.001;  // seconds (1 ms)
    constexpr TimestepDuration MINIMUM_TIMESTEP = 0.0001;
    constexpr TimestepDuration MAXIMUM_TIMESTEP = 0.1;
    
    // Default values for configuration
    constexpr SimulationStep DEFAULT_REPLAY_INTERVAL = 100;   // steps between memory replay
    constexpr SimulationStep DEFAULT_CONSOLIDATION_INTERVAL = 1000;  // steps between consolidation
    constexpr SimulationStep DEFAULT_STRUCTUREPLASTICITY_INTERVAL = 100;  // steps between structural changes
    constexpr SimulationStep DEFAULT_DEVELOPMENT_INTERVAL = 1000;  // steps between development updates
    
    // Neuron and population sizes
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;             // neurons per region
    constexpr size_t MINIMUM_NEURON_COUNT = 100;
    constexpr size_t MAXIMUM_NEURON_COUNT = 10000;
    constexpr size_t DEFAULT_REGION_COUNT = 1;
    constexpr size_t MINIMUM_REGION_COUNT = 1;
    constexpr size_t MAXIMUM_REGION_COUNT = 100;
    
    // Connectivity parameters
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;   // 10% chance of connection
    constexpr float MINIMUM_CONNECTION_PROBABILITY = 0.001f;
    constexpr float MAXIMUM_CONNECTION_PROBABILITY = 1.0f;
    constexpr float DEFAULT_INITIAL_WEIGHT_MEAN = 0.2f;
    constexpr float DEFAULT_INITIAL_WEIGHT_STD = 0.1f;
    constexpr float MINIMUM_WEIGHT = -1.0f;
    constexpr float MAXIMUM_WEIGHT = 1.0f;
    
    // Neural dynamics constants
    constexpr MembranePotential DEFAULT_V_THRESH = -50.0f;   // mV - threshold for spike
    constexpr MembranePotential DEFAULT_V_REST = -70.0f;     // mV - resting potential
    constexpr MembranePotential DEFAULT_V_RESET = -75.0f;    // mV - reset after spike
    constexpr MembranePotential DEFAULT_V_SYNAPSE_SCALE = 10.0f;  // scaling factor for synaptic current
    constexpr float DEFAULT_TAU_MEM = 20.0f;                 // ms - membrane time constant
    constexpr float DEFAULT_TAU_REF = 2.0f;                  // ms - refractory period
    
    // Spike and firing constants
    constexpr float DEFAULT_SPIKE_THRESHOLD_FACTOR = 1.5f;   // factor for spike detection
    constexpr float DEFAULT_ACTIVITY_THRESHOLD = 5.0f;      // mV - threshold for considering neuron active
    constexpr size_t DEFAULT_SPIKE_HISTORY_SIZE = 100;       // max spikes to track per synapse
    
    // Plasticity parameters (Phase 2 defaults)
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;         // STDP long-term potentiation weight
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;        // STDP long-term depression weight
    constexpr float DEFAULT_STDP_TAU = 20.0f;                // STDP time constant (ms)
    
    // Structural plasticity
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;  // probability per step per connection
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;         // probability per step per connection
    constexpr float DEFAULT_DEVELOPMENT_HIGH = 1.0f;         // high plasticity (development stage)
    constexpr float DEFAULT_DEVELOPMENT_CRITICAL = 0.8f;    // critical period plasticity
    constexpr float DEFAULT_DEVELOPMENT_MATURATION = 0.5f;  // maturation plasticity
    constexpr float DEFAULT_DEVELOPMENT_ADULT = 0.2f;        // adult stability
    constexpr float DEFAULT_DEVELOPMENT_AGING = 0.1f;        // aging reduced plasticity
    
    // Memory system constants
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;  // number of traces
    constexpr size_t DEFAULT_EPISODIC_MEMORY_CAPACITY = 1000; // number of episodes
    constexpr float DEFAULT_CONSOLIDATION_THRESHOLD = 0.3f;   // consolidation strength threshold
    constexpr SimulationStep DEFAULT_EPISODE_STORAGE_INTERVAL = 10;  // store episode every X steps
    
    // Neuromodulation constants
    constexpr float DEFAULT_DOPAMINE_LEVEL = 0.5f;           // baseline dopamine level
    constexpr float DEFAULT_DOPAMINE_EFFECTIVENESS = 0.5f;   // dopamine effect on excitability
    constexpr float DEFAULT_DOPAMINE_PLASTICITY_FACTOR = 1.0f; // neuromodulation effect on plasticity
    
    // Cognition and attention
    constexpr float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f;  // attention inhibition
    constexpr float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f;   // attention excitation
    constexpr size_t DEFAULT_PLANNING_DEPTH = 5;            // neural planner depth
    
    // Reward and learning
    constexpr float DEFAULT_REWARD_SCALE = 0.01f;            // reward scaling factor
    constexpr float DEFAULT_PREDICTION_ERROR_DEFAULT = 0.0f; // default prediction error
    
    // Visualization and debugging
    constexpr float DEFAULT_ACTIVITY_VISUALIZATION_SCALE = 20.0f; // visualization scaling
    constexpr size_t DEFAULT_SPIKE_RASTER_WINDOW = 100;      // number of spikes for raster plot
}

// System architecture constants
namespace SystemConstants {
    // Checkpoint system
    constexpr size_t DEFAULT_CHECKPOINT_MAX_FILES = 10000;
    constexpr size_t DEFAULT_CHECKPOINT_MAX_SIZE = 5 * 1024 * 1024;  // 5 MB
    constexpr bool DEFAULT_CHECKPOINT_COMPRESSION = true;
    constexpr CompressionLevel DEFAULT_CHECKPOINT_COMPRESSION_LEVEL = CompressionLevel::Balanced;
    
    // Random generation
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    
    // Development stages
    constexpr size_t DEVELOPMENT_STAGE_INITIAL = 0;
    constexpr size_t DEVELOPMENT_STAGE_CRITICAL = 1;
    constexpr size_t DEVELOPMENT_STAGE_MATURATION = 2;
    constexpr size_t DEVELOPMENT_STAGE_ADULT = 3;
    constexpr size_t DEVELOPMENT_STAGE_AGING = 4;
    
    // Configuration defaults
    constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
}

// Experimental and simulation constants
namespace ExperimentConstants {
    // Default simulation parameters
    constexpr SimulationStep DEFAULT_SIMULATION_STEPS = 1000;
    constexpr Timestamp DEFAULT_SIMULATION_DURATION = 1.0f;  // seconds
    
    // Learning experiment parameters
    constexpr size_t DEFAULT_LEARNING_EXPERIMENT_REPEATS = 100;
    constexpr float DEFAULT_LEARNING_STRENGTH_THRESHOLD = 0.01f;
    constexpr float DEFAULT_LEARNING_WEAKENING_THRESHOLD = -0.01f;
    
    // Network characteristics
    constexpr float DEFAULT_EXCITATION_INHIBITION_RATIO_MIN = 0.1f;
    constexpr float DEFAULT_EXCITATION_INHIBITION_RATIO_MAX = 10.0f;
}

// Utility functions for constants validation
inline bool isValidNeuronsCount(size_t count) {
    return count >= NeuralConstants::MINIMUM_NEURON_COUNT && 
           count <= NeuralConstants::MAXIMUM_NEURON_COUNT;
}

inline bool isValidConnectionProbability(float prob) {
    return prob >= NeuralConstants::MINIMUM_CONNECTION_PROBABILITY && 
           prob <= NeuralConstants::MAXIMUM_CONNECTION_PROBABILITY;
}

inline bool isValidTimestep(float timestep) {
    return timestep >= NeuralConstants::MINIMUM_TIMESTEP && 
           timestep <= NeuralConstants::MAXIMUM_TIMESTEP;
}

} // namespace nlm

// CONSTANTS.HPP END