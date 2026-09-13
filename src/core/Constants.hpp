#pragma once

// NLM Constants - Centralized configuration for magic numbers and hardcoded values
// 
// These values represent fundamental biological parameters and simulation constants
// that could potentially be made configurable through the NLM Config system
// but are kept here for performance reasons and as reasonable defaults.

namespace nlm {
namespace Constants {

// Neuron membrane dynamics
static constexpr float MEMBRANE_TIME_CONSTANT = 20.0f;      // Membrane time constant (ms)
static constexpr float MEMBRANE_RESISTANCE = 10.0f;        // Membrane resistance (MOhm)
static constexpr float RESTING_POTENTIAL = -70.0f;        // Resting membrane potential (mV)
static constexpr float THRESHOLD_POTENTIAL = -50.0f;      // Spike threshold (mV)
static constexpr float RESET_POTENTIAL = -75.0f;          // Reset after spike (mV)
static constexpr float LEAK_CONDUCTANCE = 0.01f;           // Membrane leak conductance

// Synapse and connectivity
static constexpr float DEFAULT_SYNAPSE_WEIGHT = 1.0f;      // Default synaptic weight
static constexpr float EXCITATORY_SYNAPSE_WEIGHT = 1.0f;  // Excitatory weight
static constexpr float INHIBITORY_SYNAPSE_WEIGHT = -1.0f; // Inhibitory weight
static constexpr float SYNAPSE_DELAY = 2.0f;              // Typical synaptic delay (ms)
static constexpr float SYNAPSE_SCALE_FACTOR = 10.0f;      // Scale factor for synaptic current

// Plasticity parameters
static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;   // LTP weight change
static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;  // LTD weight change
static constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f; // STDP time window (ms)
static constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;
static constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;
static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;

// Neuromodulation
static constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;  // Baseline dopamine level
static constexpr float DEFAULT_DOPAMINE_PEAK = 1.0f;      // Peak dopamine level
static constexpr float DEFAULT_DOPAMINE_DECAY_RATE = 0.1f; // Dopamine decay rate
static constexpr float DEFAULT_DOPAMINE_RELEASE_RATE = 1.0f;
static constexpr float DOPAMINE_PLASTICITY_FACTOR_MIN = 0.1f; // Minimum plasticity factor
static constexpr float DOPAMINE_PLASTICITY_FACTOR_MAX = 2.0f; // Maximum plasticity factor

// Developmental stages
static constexpr float DEVELOPMENT_INITIAL_PLASTICITY = 1.0f;       // Initial high plasticity
static constexpr float DEVELOPMENT_CRITICAL_PERIOD_PLASTICITY = 0.8f;
static constexpr float DEVELOPMENT_MATURATION_PLASTICITY = 0.5f;
static constexpr float DEVELOPMENT_ADULT_PLASTICITY = 0.2f;        // Adult low plasticity

// Memory systems
static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;    // Working memory capacity (neurons)
static constexpr float DEFAULT_Episodic_MEMORY_MAX_EPISODES = 1000.0f;
static constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;     // Memory consolidation threshold
static constexpr float MEMORY_REPLAY_INTERVAL = 100.0f;           // Replay every 100 steps
static constexpr float MEMORY_CONSOLIDATION_INTERVAL = 1000.0f;   // Consolidate every 1000 steps

// Sensorimotor parameters
static constexpr float DEFAULT_VISION_WIDTH = 16.0f;              // Vision dimensions
static constexpr float DEFAULT_VISION_HEIGHT = 16.0f;
static constexpr float DEFAULT_TOUCH_SENSORS = 8.0f;            // Touch sensors
static constexpr float DEFAULT_INTERNAL_SIGNALS = 4.0f;          // Internal signals
static constexpr float DEFAULT_PROPRIOCEPTION_SENSORS = 6.0f;    // Proprioception
static constexpr size_t DEFAULT_MOTOR_ACTIONS = 6;               // Motor action types

// Motor and action selection
static constexpr float DEFAULT_MOTOR_THRESHOLD = 0.5f;          // Minimum activity to trigger action
static constexpr float DEFAULT_MOTOR_EXPLORATION_CHANCE = 0.3f; // Curiosity-based exploration chance
static constexpr float DEFAULT_MOTOR_EXPLORATION_THRESHOLD = 0.5f;

// Attention and cognition
static constexpr float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f;
static constexpr float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f;
static constexpr float DEFAULT_ATTENTION_SELECTIVITY_THRESHOLD = 0.7f;
static constexpr float DEFAULT_CONCEPT_FORMATION_THRESHOLD = 0.75f;
static constexpr float DEFAULT_CONCEPT_STABILITY_THRESHOLD = 0.7f;
static constexpr float DEFAULT_CONCEPT_STABILITY_WINDOW = 5.0f;

// Prediction and planning
static constexpr float DEFAULT_PREDICTION_CONFIDENCE_THRESHOLD = 0.5f;
static constexpr float DEFAULT_PLANNING_DEPTH = 5.0f;
static constexpr float DEFAULT_GENERALIZATION_THRESHOLD = 0.5f;
static constexpr float DEFAULT_PLANNING_TIME_CONSTANT = 0.1f;

// Behavioral parameters
static constexpr float DEFAULT_EXPLORATION_DRIVE_THRESHOLD = 0.3f;
static constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.5f;
static constexpr float DEFAULT_CURIOUSITY_WEIGHT = 2.0f;
static constexpr float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f;

// Reward and reinforcement
static constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f;
static constexpr float DEFAULT_REWARD_SCALE = 1.0f;
static constexpr float DEFAULT_REWARD_MIN = -1.0f;
static constexpr float DEFAULT_REWARD_MAX = 1.0f;

// Environmental parameters
static constexpr float DEFAULT_ENVIRONMENT_WIDTH = 10.0f;
static constexpr float DEFAULT_ENVIRONMENT_HEIGHT = 10.0f;
static constexpr float DEFAULT_VISION_RANGE = 8.0f;
static constexpr float DEFAULT_AGENT_START_ENERGY = 100.0f;
static constexpr float DEFAULT_ENERGY_DECAY_RATE = 0.01f;

// Computational constants
static constexpr float EPSILON = 0.0001f;                       // Small value for floating point comparisons
static constexpr float NORMALIZATION_FACTOR = 20.0f;            // For normalizing neuron activations
static constexpr float SIMULATION_TIMESTEP = 0.001f;            // Default simulation timestep (s)
static constexpr size_t DEFAULT_MAX_SIMULATION_STEPS = 10000;  // Maximum simulation steps

// Debugging and testing
static constexpr bool DEBUG_MODE = false;                      // Enable debug logging
static constexpr float DEBUG_TIME_CONSTANT = 1.0f;             // Debug time constant

} // namespace Constants
} // namespace nlm
