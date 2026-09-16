#pragma once

// Brain.h constants
namespace nlm {

// Core simulation parameters
const size_t NEURON_COUNT_DEFAULT = 1000;                    // Default neuron count
const size_t REGION_COUNT_DEFAULT = 1;                      // Default region count

// Connectivity parameters
const float CONNECTION_PROBABILITY_DEFAULT = 0.1f;          // Default connection probability
const float EXCITATION_WEIGHT_DEFAULT = 0.2f;               // Default excitatory weight
const float INHIBITION_WEIGHT_DEFAULT = 0.1f;              // Default inhibitory weight

// Population distribution ratios
const float SENSORY_POPULATION_RATIO = 0.25f;               // 25% sensory neurons
const float INTERNAL_POPULATION_RATIO = 0.5f;              // 50% internal neurons
const float MOTOR_POPULATION_RATIO = 0.25f;                 // 25% motor neurons

// Plasticity parameters
const float DEFAULT_STDP_LTP_WEIGHT = 0.01f;                // STDP LTP weight
const float DEFAULT_STDP_LTD_WEIGHT = 0.012f;               // STDP LTD weight
const float DEFAULT_STDP_TIME_CONSTANT = 20.0f;             // STDP time constant

// Structural plasticity parameters
const float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;        // Synaptogenesis rate
const float DEFAULT_PRUNING_RATE = 0.00001f;               // Pruning rate

// Memory system parameters
const size_t WORKING_MEMORY_CAPACITY_DIVISOR = 10;          // Capacity = neuronCount / 10
const size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;           // Maximum stored episodes

// Planning parameters
const int PLANNING_DEPTH_DEFAULT = 5;                      // Default planning depth

// Neuromodulation parameters
const float ATTENTION_INHIBITION_STRENGTH_DEFAULT = 0.5f;  // Default inhibition strength
const float ATTENTION_EXCITATION_STRENGTH_DEFAULT = 1.5f; // Default excitation strength

// Checkpoint system parameters
const CompressionLevel CHECKPOINT_COMPRESSION_DEFAULT = CompressionLevel::Balanced; // Default compression

// Episode management parameters
const size_t EPISODE_STORE_INTERVAL = 10;                   // Store episode every 10 steps

// Developmental parameters
const SimulationStep DEVELOPMENT_UPDATE_INTERVAL = 1000;    // Update development every 1000 steps

// Memory consolidation parameters
const float EPISODIC_CONSOLIDATION_THRESHOLD = 0.3f;          // Consolidation threshold

// Action selection parameters
const float MOTOR_ACTIVATION_THRESHOLD = 0.5f;               // Minimum firing to trigger action

// Learning parameters
const float PLASTICITY_LEARNING_RATE_DEFAULT = 0.01f;       // Default learning rate
const float MAX_SYNAPSE_WEIGHT = 1.0f;                      // Maximum synapse weight
const float MIN_SYNAPSE_WEIGHT = -1.0f;                     // Minimum synapse weight

// Reward and error parameters
const float REWARD_SCALE_FACTOR = 1.0f;                     // Reward scale factor
const float ERROR_SCALE_FACTOR = 1.0f;                      // Error scale factor

// Activation thresholds
const float ACTIVATION_THRESHOLD = 0.5f;                    // Neural activation threshold
const float NOISE_THRESHOLD = 0.3f;                         // Plasticity noise threshold

// Developmental stage age thresholds
const double DEVELOPMENTAL_STAGE_INITIAL = 60.0;            // ~1 minute
const double DEVELOPMENTAL_STAGE_CRITICAL_PERIOD = 300.0;   // ~5 minutes
const double DEVELOPMENTAL_STAGE_MATURATION = 900.0;        // ~15 minutes

// Simulation timing
const double SIMULATION_TIMESTEP_DEFAULT = 0.001;           // 1ms timestep
const SimulationStep REPLAY_INTERVAL_DEFAULT = 100;        // Replay every 100 steps
const SimulationStep CONSOLIDATION_INTERVAL_DEFAULT = 1000; // Consolidate every 1000 steps

} // namespace nlm
