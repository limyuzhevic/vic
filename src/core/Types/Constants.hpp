#pragma once

// NLM Magic Numbers - Documented and Named Constants
// 
// This file contains named constants for all magic numbers used throughout the NLM codebase.
// This improves code readability, maintainability, and makes it easier to tune the system.

namespace nlm {

// ========== NEURON DYNAMICS ==========
// Membrane potential thresholds (in mV)
constexpr float NEURON_THRESHOLD_POTENTIAL = -50.0f;        // Action potential threshold
constexpr float NEURON_RESTING_POTENTIAL = -70.0f;         // Resting membrane potential
constexpr float NEURON_RESET_POTENTIAL = -75.0f;           // Reset after spike
constexpr float NEURON_MEMBRANE_FLUX_FACTOR = 10.0f;       // For working memory storage
constexpr float NEURON_ACTIVATION_THRESHOLD = 5.0f;        // For episodic memory (absolute difference)

// Time constants (in ms)
constexpr float NEURON_MEMBRANE_TIME_CONSTANT = 20.0f;    // Membrane time constant tau_m
constexpr float NEURON_REFACTORY_TIME = 2.0f;              // Refractory period
constexpr float NEURON_SPIKE_WINDOW = 2.0f * 0.001f;        // Window for detecting recent spikes

// Input scaling factors
constexpr float SENSORY_VISION_INPUT_SCALE = 5.0f;         // Vision intensity -> current
constexpr float SENSORY_TOUCH_INPUT_SCALE = 8.0f;         // Collision signal -> current
constexpr float SENSORY_INTERNAL_INPUT_SCALE = 5.0f;       // Internal signals -> current
constexpr float SENSORY_PROPRIOCEPTION_SCALE = 3.0f;      // Proprioception -> current

// ========== PLASTICITY ==========
// STDP parameters
constexpr float STDP_LTP_WEIGHT = 0.02f;                   // Long-Term Potentiation weight change
constexpr float STDP_LTD_WEIGHT = 0.015f;                  // Long-Term Depression weight change
constexpr float STDP_TIME_CONSTANT = 20.0f;                // STDP time constant tau
constexpr float STDP_LEARNING_THRESHOLD = 0.01f;           // Minimum weight change to count

// Hebbian learning
constexpr float HEBBIAN_LEARNING_RATE = 0.001f;           // Base learning rate for Hebbian

// Structural plasticity
constexpr float SYNAPTOGENESIS_RATE_INITIAL = 0.0001f;    // New synapse formation rate (initial)
constexpr float PRUNING_RATE_INITIAL = 0.00001f;           // Weak synapse removal rate (initial)
constexpr float STRUCTURAL_PLACEMENT_INTERVAL = 100;      // Steps between structural updates

// General plasticity
constexpr float NEUROMODULATION_PLASTICITY_FACTOR_RANGE = 2.0f;  // Range of plasticity modulation
constexpr float NEUROMODULATION_EXCITABILITY_FACTOR = 0.5f;     // Dopamine -> excitability mapping
constexpr float PLASTICITY_MIN_FACTOR = 0.1f;                  // Minimum plasticity factor
constexpr float PLASTICITY_MAX_FACTOR = 2.0f;                  // Maximum plasticity factor

// ========== MEMORY SYSTEMS ==========
// Working memory
constexpr float WORKING_MEMORY_UPDATE_TIMESTEP = 0.001f;   // Working memory update step
constexpr float WORKING_MEMORY_NORMALIZATION_DIVISOR = 10.0f; // For neuron activity normalization

// Episodic memory
constexpr size_t EPISODE_STORAGE_INTERVAL = 10;            // Store episode every N steps
constexpr float EPISODE_ACTIVATION_NORMALIZATION = 20.0f;   // For neuron activation scaling

// Memory replay and consolidation
constexpr size_t REPLAY_INTERVAL_DEFAULT = 100;             // Default replay interval
constexpr size_t REPLAY_EPISODES_COUNT = 3;                 // Number of episodes to replay
constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;      // Consolidation threshold

// ========== NEUROMODULATION ==========
// Dopamine
constexpr float DOPAMINE_CLAMP_MIN = -1.0f;                // Minimum dopamine level
constexpr float DOPAMINE_CLAMP_MAX = 1.0f;                  // Maximum dopamine level
constexpr float DOPAMINE_PREDICTION_ERROR_WEIGHT = 1.0f;    // PE -> dopamine mapping

// Curiosity and exploration
constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;           // Novelty contribution to curiosity
constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f;  // PE contribution to curiosity
constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;    // Minimum curiosity for exploration
constexpr float CURIOSITY_EXPLORATION_CHANCE_MAX = 0.3f;   // Maximum exploration probability
constexpr float CURIOSITY_NOVELTY_DECAY = 0.99f;           // Novelty decay factor

// Novelty detection
constexpr float NOVELTY_SENSITIVITY = 0.01f;               // Novelty detection sensitivity

// ========== DEVELOPMENT ==========
// Developmental stages and plasticity modifiers
constexpr float DEVELOPMENT_STAGE_INITIAL_PLATONICITY = 1.0f;   // Initial high plasticity
constexpr float DEVELOPMENT_STAGE_CRITICAL_PERIOD_PLATONICITY = 0.8f;
constexpr float DEVELOPMENT_STAGE_MATURATION_PLATONICITY = 0.5f;
constexpr float DEVELOPMENT_STAGE_ADULT_PLATONICITY = 0.2f;     // Adult stable state

// Development timing
constexpr double DEVELOPMENT_AGE_INITIAL = 60.0;           // Age for Initial stage (~1 minute)
constexpr double DEVELOPMENT_AGE_CRITICAL_PERIOD = 300.0;  // Age for CriticalPeriod (~5 minutes)
constexpr double DEVELOPMENT_AGE_MATURATION = 900.0;       // Age for Maturation (~15 minutes)
constexpr float DEVELOPMENT_UPDATE_INTERVAL_INITIAL = 0.001f;   // Initial development timestep
constexpr float DEVELOPMENT_UPDATE_INTERVAL_ADULT = 1000f * 0.001f; // Adult development timestep (1000 steps)

// ========== WORLD AND ENVIRONMENT ==========
// World dimensions
constexpr size_t WORLD_VISION_WIDTH = 16;                 // Vision width
constexpr size_t WORLD_VISION_HEIGHT = 16;                // Vision height
constexpr size_t WORLD_TOUCH_POINTS = 8;                  // Touch sensor count
constexpr size_t WORLD_INTERNAL_SIGNALS = 4;               // Internal signal count
constexpr size_t WORLD_PROPRIOCEPTION_POINTS = 6;         // Proprioception sensor count

// Motor commands
constexpr float MOTOR_ACTION_THRESHOLD = 0.5f;             // Minimum activity for action selection
constexpr size_t MOTOR_COMMAND_RANDOM_RANGE = 7;          // For curiosity-based random actions

// Reward and reinforcement
constexpr float REWARD_DISCOUNT_FACTOR = 0.99f;           // Reward discounting
constexpr float REWARD_PREDICTION_UPDATE_ALPHA = 0.05f;   // Moving average update rate

// ========== EXPERIMENTS AND TESTING ==========
// Test parameters
constexpr size_t BASIC_CONNECTIVITY_TEST_STEPS = 50;      // Steps for basic connectivity test
constexpr size_t PLASTICITY_EXPERIMENT_STEPS = 1000;       // Steps for plasticity experiment
constexpr size_t STDP_VERIFICATION_TRIALS = 50;            // Trials for STDP verification

// Demo and integration parameters
constexpr size_t PHASE3_DEMO_NEURONS = 500;                // Neurons for Phase 3 demo
constexpr size_t PHASE4_DEMO_NEURONS = 1000;               // Neurons for Phase 4 demo
constexpr size_t PHASE6_DEMO_NEURONS = 500;                // Neurons for Phase 6 demo
constexpr size_t PHASE6_DEMO_MAX_STEPS = 2000;              // Max steps for Phase 6 demo

// ========== PERFORMANCE AND OPTIMIZATION ==========
// Numerical precision and clamping
constexpr float FLOAT_EPSILON = 0.001f;                    // Small value for floating point comparisons
constexpr float EPSILON_FOR_WEIGHT_CHANGE = 0.001f;       // Minimum significant weight change

// Memory and performance
constexpr size_t PERFORMANCE_METRICS_BUFFER_SIZE = 1000; // Size of performance metric buffers
constexpr float PERFORMANCE_UPDATE_INTERVAL = 100.0;      // Performance update interval

// ========== MISCELLANEOUS ==========
// Configuration defaults
constexpr int64_t DEFAULT_RANDOM_SEED = 42;                // Default random seed
constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;     // Default simulation timestep (seconds)
constexpr size_t DEFAULT_NEURON_COUNT = 1000;             // Default number of neurons
constexpr size_t DEFAULT_REGION_COUNT = 1;                // Default number of regions
constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;   // Default connectivity probability

// Logging and output
constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log";  // Default log filename
constexpr size_t LOG_MESSAGE_BUFFER_SIZE = 4096;         // Log message buffer size

// File and system
constexpr const char* CHECKPOINT_FILE_EXTENSION = ".bin";  // Checkpoint file extension
constexpr double MAX_WALL_CLOCK_TIME = 3600.0;             // Maximum simulation time (seconds)

// ========== OBSERVATION ==========
// Observation system constants
constexpr float OBSERVATION_SCALE_FACTOR = 2.0f;           // Scale factor for observation normalization
constexpr float OBSERVATION_CENTER_OFFSET = 1.0f;         // Center offset for observation signals

// Status and diagnostic constants
constexpr float STATUS_DEFAULT_VALUE = 0.0f;               // Default value for status variables
constexpr size_t STATUS_COUNT_DEFAULT = 0;                // Default value for status counts

} // namespace nlm
