// NLM Configuration Constants Implementation
// 
// This file provides explicit definitions for configuration constants
// used throughout the NLM brain simulation. While the header file provides
// the constant declarations, this implementation file ensures proper
// initialization and provides a centralized location for constant definitions.
//
// Constants are organized by functionality:
// - Time constants (simulation steps, intervals)
// - Neural parameters (thresholds, rates, weights)
// - Plasticity parameters
// - Memory system parameters
// - Development parameters
// - Integration parameters
//
// This implementation follows the principle of "header-only for constants"
// by providing explicit definitions only when needed, reducing compilation
// dependencies while maintaining the benefits of named constants.

#include "core/config/Constants.hpp"
#include <cmath>
#include <limits>

namespace nlm {
namespace config {

// === TIME CONSTANTS ===

// Simulation timestep (seconds)
double SIMULATION_TIMESTEP = 0.001;  // 1ms

// Integration intervals (steps)
size_t CHECKPOINT_INTERVAL = 1000;    // Save checkpoint every 1000 steps
size_t REPLAY_INTERVAL = 100;         // Replay episodes every 100 steps
size_t CONSOLIDATION_INTERVAL = 1000; // Consolidate memory every 1000 steps
size_t DEVELOPMENT_UPDATE_INTERVAL = 1000; // Update development every 1000 steps

// Spike timing constants
double SPIKE_DETECTION_THRESHOLD = 0.002; // 2ms after spike to detect
double REFACTORY_PERIOD_MIN = 2.0;  // ms
double REFACTORY_PERIOD_MAX = 10.0; // ms

// === NEURAL PARAMETERS ===

// Neuron membrane potentials
double NEURON_RESTING_POTENTIAL = -70.0;  // mV
double NEURON_THRESHOLD = -55.0;          // mV
double NEURON_RESET_POTENTIAL = -70.0;   // mV
double NEURON_MEMBRANE_CAPACITANCE = 1.0; // nF
double NEURON_TIME_CONSTANT = 20.0;      // ms
double NEURON_LEAK_CONDUCTANCE_DEFAULT = 0.04; // per ms

// Neuronal input/output ranges
double SENSORY_INPUT_MIN = -10.0;  // mV
double SENSORY_INPUT_MAX = 10.0;   // mV
double MOTOR_OUTPUT_MIN = 0.0;     // arbitrary units
double MOTOR_OUTPUT_MAX = 1.0;     // arbitrary units

// Spike-related parameters
double SPIKE_AMPLITUDE = 25.0;    // mV jump during spike
size_t MAX_SPIKE_HISTORY = 100;   // Number of past spikes to store
double ADAPTATION_RATE = 0.01f;   // Spike-frequency adaptation rate

// === SYNAPTIC PARAMETERS ===

// Synaptic weight ranges
double SYNAPTIC_WEIGHT_MIN = -1.0;  // Normalized
double SYNAPTIC_WEIGHT_MAX = 1.0;   // Normalized
double EXCITATORY_WEIGHT_MIN = 0.0; // Excitatory only positive
double EXCITATORY_WEIGHT_MAX = 0.5; // Typical max for excitatory
double INHIBITORY_WEIGHT_MIN = -0.5; // Typical min for inhibitory
double INHIBITORY_WEIGHT_MAX = 0.0;  // Inhibitory only negative

// Synaptic delay
size_t SYNAPTIC_DELAY_MIN = 1;    // steps (minimum)
size_t SYNAPTIC_DELAY_MAX = 5;    // steps (maximum)

// Synaptic transmission
double SYNAPTIC_REVERSAL_EXCITATORY = 0.0;   // mV
double SYNAPTIC_REVERSAL_INHIBITORY = -70.0; // mV
double SYNAPTIC_CONDUCTANCE_SCALE = 10.0;    // Scaling factor for synaptic current

// === PLASTICITY PARAMETERS ===

// STDP parameters
double STDP_LTP_WEIGHT = 0.02f;    // Long-term potentiation
double STDP_LTD_WEIGHT = 0.015f;   // Long-term depression
double STDP_TIME_CONSTANT = 20.0f; // ms (tau)
double STDP_TRACE_DECAY = 0.01f;   // Eligibility trace decay per step

// Hebbian learning
double HEBBIAN_LEARNING_RATE = 0.01f; // Learning rate for co-activation

// Reward-modulated plasticity
double REWARD_PLASTICITY_GAIN = 0.001f;   // Scaling factor for reward influence

// Structural plasticity
double SYNAPTOGENESIS_RATE = 0.0001f;      // Rate of new synapse formation
double PRUNING_RATE = 0.00001f;            // Rate of synapse removal

// === MEMORY SYSTEM PARAMETERS ===

// Working memory
double WORKING_MEMORY_DECAY_RATE = 0.95f;    // Decay per timestep
double WORKING_MEMORY_COMPETITION_STRENGTH = 1.0f; // Competition strength

// Episodic memory
double EPISODIC_REPLAY_STRENGTH = 1.0f;      // Replay effectiveness
double EPISODIC_CONSOLIDATION_THRESHOLD = 0.3f; // Confidence threshold for consolidation

// Associative memory
double ASSOCIATIVE_ASSOCIATION_STRENGTH = 0.5f; // Strength of associations
double ASSOCIATIVE_FORGETTING_RATE = 0.999f;    // Decay rate

// === NEUROMODULATION PARAMETERS ===

// Dopamine
double DOPAMINE_BASELINE = 0.0f;            // Baseline level
double DOPAMINE_PEAK = 1.0f;                // Maximum level
double DOPAMINE_DECAY_RATE = 0.99f;         // Decay per timestep
double DOPAMINE_EFFECT_ON_EXCITABILITY = 0.5f; // Scaling factor
double DOPAMINE_EFFECT_ON_PLASTICITY = 1.0f;   // Scaling factor for plasticity

// Curiosity
double CURIOSITY_THRESHOLD = 0.1f;          // Novelty threshold for curiosity
double CURIOSITY_DECAY_RATE = 0.98f;        // Decay per timestep
double CURIOSITY_EXPLORATION_GAIN = 0.5f;  // Effect on exploration behavior

// Novelty
double NOVELTY_SENSITIVITY = 1.0f;          // Sensitivity to novel patterns
double NOVELTY_DECAY_RATE = 0.95f;          // Decay per timestep

// Prediction error
double PREDICTION_ERROR_WEIGHT = 0.5f;      // Influence on learning
double PREDICTION_ERROR_DECAY = 0.9f;       // Decay per timestep

// === DEVELOPMENT PARAMETERS ===

// Developmental stages
double DEVELOPMENT_STAGE_INITIAL = 1.0f;    // High plasticity
double DEVELOPMENT_STAGE_CRITICAL_PERIOD = 0.8f; // Reduced plasticity
double DEVELOPMENT_STAGE_MATURATION = 0.5f;   // Further reduced plasticity
double DEVELOPMENT_STAGE_ADULT = 0.2f;      // Stable plasticity

// Developmental effects
double DEVELOPMENT_PLASTICITY_DECAY = 0.99f; // Plasticity decay over development
double DEVELOPMENT_STRUCTURE_MATURATION = 0.8f; // Structural maturity

// === INTEGRATION PARAMETERS ===

// System integration
double INTEGRATION_NOVELTY_THRESHOLD = 0.3f;   // Threshold for detecting novel patterns
size_t INTEGRATION_MAX_EPISODES_FOR_REPLAY = 3; // Number of episodes to replay

// Performance optimization
double MIN_NEURONS_PER_POPULATION = 10.0;      // Minimum neurons per population
double MAX_NEURONS_PER_POPULATION = 1000000.0; // Maximum neurons per population

// === CONFIGURATION DEFAULT VALUES ===

// Brain configuration defaults
double DEFAULT_RANDOM_SEED = 42.0;
double DEFAULT_NEURON_COUNT = 1000.0;         // Base neuron count
double DEFAULT_REGION_COUNT = 1.0;           // Base region count
double DEFAULT_CONNECTION_PROBABILITY = 0.1f; // Connection probability
double DEFAULT_STDP_LTP_WEIGHT = STDP_LTP_WEIGHT;
double DEFAULT_STDP_LTD_WEIGHT = STDP_LTD_WEIGHT;
double DEFAULT_STDP_TAU = STDP_TIME_CONSTANT;

// Memory system defaults
double DEFAULT_WORKING_MEMORY_CAPACITY = 100.0;
double DEFAULT_MAX_EPISODES = 1000.0;

// === SYSTEM THRESHOLDS ===

// Activity thresholds
double SPIKE_THRESHOLD_CURRENT = 10.0f;      // mV to trigger spike
double EXCITATORY_SYNAPSE_THRESHOLD = 0.0f; // Excitatory if > 0
double INHIBITORY_SYNAPSE_THRESHOLD = 0.0f; // Inhibitory if < 0

// Learning thresholds
double PLASTICITY_MIN_DELTA = 0.01f;         // Minimum weight change to consider
double PLASTICITY_CONVERGENCE_THRESHOLD = 0.001f; // Convergence threshold

// Integration thresholds
double INTEGRATION_MEMORY_THRESHOLD = 0.5f;  // Memory integration confidence
double INTEGRATION_NEUROMODULATION_THRESHOLD = 0.3f; // Neuromodulation integration confidence

// === MATHEMATICAL CONSTANTS ===

// Mathematical constants used throughout the simulation
double PI = 3.14159265358979323846f;
double E = 2.71828182845904523536f;
double SQRT2 = 1.41421356237309504880f;
double GOLDEN_RATIO = 1.61803398874989484820f;

} // namespace config
} // namespace nlm
