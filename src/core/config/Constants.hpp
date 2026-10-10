#pragma once

/**
 * Configuration Constants for NLM (Neural Learning Machine)
 * 
 * This file provides named constants for all magic numbers and configuration
 * values used throughout the NLM brain simulation. This improves code readability,
 * maintainability, and makes it easier to tune system parameters.
 * 
 * Constants are grouped by functionality:
 * - Time constants (simulation steps, intervals)
 * - Neural parameters (thresholds, rates, weights)
 * - Plasticity parameters
 * - Memory system parameters
 * - Development parameters
 * - Integration parameters
 */

namespace nlm {
namespace config {

// === TIME CONSTANTS ===

// Simulation timestep (seconds)
static constexpr double SIMULATION_TIMESTEP = 0.001;  // 1ms

// Integration intervals (steps)
static constexpr size_t CHECKPOINT_INTERVAL = 1000;    // Save checkpoint every 1000 steps
static constexpr size_t REPLAY_INTERVAL = 100;         // Replay episodes every 100 steps
static constexpr size_t CONSOLIDATION_INTERVAL = 1000; // Consolidate memory every 1000 steps
static constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000; // Update development every 1000 steps

// Spike timing constants
static constexpr double SPIKE_DETECTION_THRESHOLD = 0.002; // 2ms after spike to detect
static constexpr double REFACTORY_PERIOD_MIN = 2.0;  // ms
static constexpr double REFACTORY_PERIOD_MAX = 10.0; // ms

// === NEURAL PARAMETERS ===

// Neuron membrane potentials
static constexpr double NEURON_RESTING_POTENTIAL = -70.0;  // mV
static constexpr double NEURON_THRESHOLD = -55.0;          // mV
static constexpr double NEURON_RESET_POTENTIAL = -70.0;   // mV
static constexpr double NEURON_MEMBRANE_CAPACITANCE = 1.0; // nF
static constexpr double NEURON_TIME_CONSTANT = 20.0;      // ms
static constexpr double NEURON_LEAK_CONDUCTANCE_DEFAULT = 0.04; // per ms

// Neuronal input/output ranges
static constexpr double SENSORY_INPUT_MIN = -10.0;  // mV
static constexpr double SENSORY_INPUT_MAX = 10.0;   // mV
static constexpr double MOTOR_OUTPUT_MIN = 0.0;     // arbitrary units
static constexpr double MOTOR_OUTPUT_MAX = 1.0;     // arbitrary units

// Spike-related parameters
static constexpr double SPIKE_AMPLITUDE = 25.0;    // mV jump during spike
static constexpr size_t MAX_SPIKE_HISTORY = 100;   // Number of past spikes to store
static constexpr double ADAPTATION_RATE = 0.01f;   // Spike-frequency adaptation rate

// === SYNAPTIC PARAMETERS ===

// Synaptic weight ranges
static constexpr double SYNAPTIC_WEIGHT_MIN = -1.0;  // Normalized
static constexpr double SYNAPTIC_WEIGHT_MAX = 1.0;   // Normalized
static constexpr double EXCITATORY_WEIGHT_MIN = 0.0; // Excitatory only positive
static constexpr double EXCITATORY_WEIGHT_MAX = 0.5; // Typical max for excitatory
static constexpr double INHIBITORY_WEIGHT_MIN = -0.5; // Typical min for inhibitory
static constexpr double INHIBITORY_WEIGHT_MAX = 0.0;  // Inhibitory only negative

// Synaptic delay
static constexpr size_t SYNAPTIC_DELAY_MIN = 1;    // steps (minimum)
static constexpr size_t SYNAPTIC_DELAY_MAX = 5;    // steps (maximum)

// Synaptic transmission
static constexpr double SYNAPTIC_REVERSAL_EXCITATORY = 0.0;   // mV
static constexpr double SYNAPTIC_REVERSAL_INHIBITORY = -70.0; // mV
static constexpr double SYNAPTIC_CONDUCTANCE_SCALE = 10.0;    // Scaling factor for synaptic current

// === PLASTICITY PARAMETERS ===

// STDP parameters
static constexpr double STDP_LTP_WEIGHT = 0.02f;    // Long-term potentiation
static constexpr double STDP_LTD_WEIGHT = 0.015f;   // Long-term depression
static constexpr double STDP_TIME_CONSTANT = 20.0f; // ms (tau)
static constexpr double STDP_TRACE_DECAY = 0.01f;   // Eligibility trace decay per step

// Hebbian learning
static constexpr double HEBBIAN_LEARNING_RATE = 0.01f; // Learning rate for co-activation

// Reward-modulated plasticity
static constexpr double REWARD_PLASTICITY_GAIN = 0.001f;   // Scaling factor for reward influence

// Structural plasticity
static constexpr double SYNAPTOGENESIS_RATE = 0.0001f;      // Rate of new synapse formation
static constexpr double PRUNING_RATE = 0.00001f;            // Rate of synapse removal

// === MEMORY SYSTEM PARAMETERS ===

// Working memory
static constexpr size_t WORKING_MEMORY_CAPACITY_FACTOR = 10; // neurons / working_memory_size
static constexpr double WORKING_MEMORY_DECAY_RATE = 0.95f;    // Decay per timestep
static constexpr double WORKING_MEMORY_COMPETITION_STRENGTH = 1.0f; // Competition strength

// Episodic memory
static constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;  // Maximum stored episodes
static constexpr double EPISODIC_REPLAY_STRENGTH = 1.0f;      // Replay effectiveness
static constexpr double EPISODIC_CONSOLIDATION_THRESHOLD = 0.3f; // Confidence threshold for consolidation

// Associative memory
static constexpr double ASSOCIATIVE_ASSOCIATION_STRENGTH = 0.5f; // Strength of associations
static constexpr double ASSOCIATIVE_FORGETTING_RATE = 0.999f;    // Decay rate

// === NEUROMODULATION PARAMETERS ===

// Dopamine
static constexpr double DOPAMINE_BASELINE = 0.0f;            // Baseline level
static constexpr double DOPAMINE_PEAK = 1.0f;                // Maximum level
static constexpr double DOPAMINE_DECAY_RATE = 0.99f;         // Decay per timestep
static constexpr double DOPAMINE_EFFECT_ON_EXCITABILITY = 0.5f; // Scaling factor
static constexpr double DOPAMINE_EFFECT_ON_PLASTICITY = 1.0f;   // Scaling factor for plasticity

// Curiosity
static constexpr double CURIOSITY_THRESHOLD = 0.1f;          // Novelty threshold for curiosity
static constexpr double CURIOSITY_DECAY_RATE = 0.98f;        // Decay per timestep
static constexpr double CURIOSITY_EXPLORATION_GAIN = 0.5f;  // Effect on exploration behavior

// Novelty
static constexpr double NOVELTY_SENSITIVITY = 1.0f;          // Sensitivity to novel patterns
static constexpr double NOVELTY_DECAY_RATE = 0.95f;          // Decay per timestep

// Prediction error
static constexpr double PREDICTION_ERROR_WEIGHT = 0.5f;      // Influence on learning
static constexpr double PREDICTION_ERROR_DECAY = 0.9f;       // Decay per timestep

// === DEVELOPMENT PARAMETERS ===

// Developmental stages
static constexpr double DEVELOPMENT_STAGE_INITIAL = 1.0f;    // High plasticity
static constexpr double DEVELOPMENT_STAGE_CRITICAL_PERIOD = 0.8f; // Reduced plasticity
static constexpr double DEVELOPMENT_STAGE_MATURES = 0.5f;   // Further reduced plasticity
static constexpr double DEVELOPMENT_STAGE_ADULT = 0.2f;      // Stable plasticity

// Developmental effects
static constexpr double DEVELOPMENT_PLASTICITY_DECAY = 0.99f; // Plasticity decay over development
static constexpr double DEVELOPMENT_STRUCTURE_MATURATION = 0.8f; // Structural maturity

// === INTEGRATION PARAMETERS ===

// System integration
static constexpr size_t INTEGATION_CHECKPOINT_INTERVAL = 100;  // Integration checkpoint frequency
static constexpr double INTEGRATION_NOVELTY_THRESHOLD = 0.3f;   // Threshold for detecting novel patterns
static constexpr size_t INTEGRATION_MAX_EPISODES_FOR_REPLAY = 3; // Number of episodes to replay

// Performance optimization
static constexpr size_t MIN_NEURONS_PER_POPULATION = 10;      // Minimum neurons per population
static constexpr size_t MAX_NEURONS_PER_POPULATION = 1000000; // Maximum neurons per population

// === CONFIGURATION DEFAULT VALUES ===

// Brain configuration defaults
static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
static constexpr size_t DEFAULT_NEURON_COUNT = 1000;         // Base neuron count
static constexpr size_t DEFAULT_REGION_COUNT = 1;           // Base region count
static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f; // Connection probability
static constexpr float DEFAULT_STDP_LTP_WEIGHT = STDP_LTP_WEIGHT;
static constexpr float DEFAULT_STDP_LTD_WEIGHT = STDP_LTD_WEIGHT;
static constexpr float DEFAULT_STDP_TAU = STDP_TIME_CONSTANT;

// Memory system defaults
static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
static constexpr size_t DEFAULT_MAX_EPISODES = 1000;

// === SYSTEM THRESHOLDS ===

// Activity thresholds
static constexpr float SPIKE_THRESHOLD_CURRENT = 10.0f;      // mV to trigger spike
static constexpr float EXCITATORY_SYNAPSE_THRESHOLD = 0.0f; // Excitatory if > 0
static constexpr float INHIBITORY_SYNAPSE_THRESHOLD = 0.0f; // Inhibitory if < 0

// Learning thresholds
static constexpr float PLASTICITY_MIN_DELTA = 0.01f;         // Minimum weight change to consider
static constexpr float PLASTICITY_CONVERGENCE_THRESHOLD = 0.001f; // Convergence threshold

// Integration thresholds
static constexpr float INTEGRATION_MEMORY_THRESHOLD = 0.5f;  // Memory integration confidence
static constexpr float INTEGRATION_NEUROMODULATION_THRESHOLD = 0.3f; // Neuromodulation integration confidence

} // namespace config
} // namespace nlm
