// NLM (Neural Learning Machine) Constants Header
// Centralized constants for improved code maintainability and readability
// Provides named constants for magic numbers throughout the NLM codebase

#pragma once

namespace nlm {

// ==============================================================================
// TIME AND SIMULATION CONSTANTS
// ==============================================================================

// Core simulation timestep (seconds)
inline constexpr double SIMULATION_TIMESTEP = 0.001;  // 1 ms
typedef std::chrono::duration<double> TimestepDuration;

// Update intervals (in simulation steps)
inline constexpr size_t STDP_UPDATE_INTERVAL = 1;  // STDP updates per step
inline constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000;  // Development updates
inline constexpr size_t REPLAY_INTERVAL = 100;  // Memory replay interval
inline constexpr size_t CONSOLIDATION_INTERVAL = 1000;  // Memory consolidation interval
inline constexpr size_t EPISODE_STORE_INTERVAL = 10;  // Episode storage interval

// ==============================================================================
// NEURAL CONSTANTS
// ==============================================================================

// Neuron dynamics constants
inline constexpr float NEURON_THRESHOLD_DEFAULT = 15.0f;  // Membrane potential threshold
inline constexpr float NEURON_RESTING_POTENTIAL = -70.0f;  // Resting membrane potential
inline constexpr float NEURON_RESET_POTENTIAL = -65.0f;   // Reset membrane potential
inline constexpr float NEURON_LEAK_CONDUCTANCE = 0.1f;     // Membrane leak conductance
inline constexpr float NEURON_REFECTORY_PERIOD = 0.002f;  // Refractory period (s)

// Synapse constants
inline constexpr float MAX_SYNAPTIC_WEIGHT = 10.0f;      // Maximum synaptic weight
inline constexpr float MIN_SYNAPTIC_WEIGHT = -10.0f;     // Minimum synaptic weight
inline constexpr float STDP_LTP_WEIGHT_DEFAULT = 0.01f; // STDP LTP weight
inline constexpr float STDP_LTD_WEIGHT_DEFAULT = 0.012f; // STDP LTD weight
inline constexpr float STDP_TIME_CONSTANT = 20.0f;       // STDP time constant (ms)
inline constexpr float HEBBIAN_LEARNING_RATE = 0.01f;    // Hebbian learning rate

// Population constants
inline constexpr float SENSORY_POPULATION_RATIO = 0.25f; // 25% sensory neurons
inline constexpr float INTERNAL_POPULATION_RATIO = 0.5f;  // 50% internal neurons
inline constexpr float MOTOR_POPULATION_RATIO = 0.25f;     // 25% motor neurons

// ==============================================================================
// ENVIRONMENT CONSTANTS
// ==============================================================================

// Grid world environment constants
inline constexpr int ENVIRONMENT_WIDTH = 10;
inline constexpr int ENVIRONMENT_HEIGHT = 10;
inline constexpr float ENVIRONMENT_GOAL_REWARD = 1.0f;     // Reward for reaching goal
inline constexpr float ENVIRONMENT_STEP_REWARD = -0.01f;   // Penalty per step
inline constexpr int ENVIRONMENT_MAX_STEPS = 1000;        // Maximum steps per episode
inline constexpr size_t ENVIRONMENT_ACTION_SPACE_SIZE = 4; // 4 movement actions
inline constexpr size_t ENVIRONMENT_OBSERVATION_SPACE_SIZE = 4; // Position, goal, etc.

// Motor system constants
inline constexpr float MOTOR_NOISE_LEVEL = 0.1f;           // Motor output noise
inline constexpr float MOTOR_DEAD_ZONE = 0.01f;           // Minimum motor activation

// ==============================================================================
// MEMORY CONSTANTS
// ==============================================================================

// Working memory constants
inline constexpr size_t WORKING_MEMORY_CAPACITY_DEFAULT = 100;  // Default working memory capacity
inline constexpr float WORKING_MEMORY_DECAY_RATE = 0.9f;          // Working memory decay
inline constexpr float MEMORY_COMPETITION_FACTOR = 1.0f;         // Memory competition strength

// Episodic memory constants
inline constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;     // Maximum stored episodes
inline constexpr float EPISODE_SIMILARITY_THRESHOLD = 0.7f;      // Memory replay threshold
inline constexpr size_t EPISODE_REPLAY_COUNT = 3;                // Number of episodes to replay

// Associative memory constants
inline constexpr float ASSOCIATIVE_LEARNING_RATE = 0.05f;       // Associative memory learning rate
inline constexpr size_t ASSOCIATIVE_PATTERN_DIM = 10;          // Pattern dimension

// ==============================================================================
// DEVELOPMENT CONSTANTS
// ==============================================================================

// Synaptogenesis (neural growth) constants
inline constexpr float SYNAPTAGENESIS_RATE_MAIN = 0.0001f;      // Main synaptogenesis rate
inline constexpr float SYNAPTAGENESIS_RATE_EARLY = 0.0005f;     // Early development rate
inline constexpr float SYNAPTAGENESIS_RATE_LATE = 0.00005f;     // Late development rate

// Pruning (neural elimination) constants
inline constexpr float PRUNING_RATE_MAIN = 0.00001f;           // Main pruning rate
inline constexpr float PRUNING_RATE_EARLY = 0.000005f;        // Early development pruning

// Developmental stage constants
inline constexpr double DEVELOPMENT_STAGE_INITIAL = 0.0;      // Initial stage age (s)
inline constexpr double DEVELOPMENT_STAGE_CRITICAL = 60.0;   // Critical period (s)
inline constexpr double DEVELOPMENT_STAGE_MATURATION = 300.0; // Maturation period (s)
inline constexpr double DEVELOPMENT_STAGE_ADULT = 900.0;     // Adult stage (s)

// Plasticity modulation constants
inline constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;    // High initial plasticity
inline constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;   // Critical period modulation
inline constexpr float PLASTICITY_MODIFIER_MATURE = 0.5f;     // Mature modulation
inline constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;      // Adult stability

// ==============================================================================
// NEUROMODULATION CONSTANTS
// ==============================================================================

// Dopamine constants
inline constexpr float DOPAMINE_CLAMP_MIN = -1.0f;           // Minimum dopamine level
inline constexpr float DOPAMINE_CLAMP_MAX = 1.0f;            // Maximum dopamine level
inline constexpr float DOPAMINE_BASELINE = 0.0f;             // Baseline dopamine
inline constexpr float DOPAMINE_REWARD_PREDICTION_ERROR_GAIN = 1.0f; // RPE gain

// Curiosity constants
inline constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;   // Threshold for exploratory behavior
inline constexpr float CURIOSITY_EXPLORATION_FACTOR = 0.5f;    // Curiosity-based exploration factor

// Novelty constants
inline constexpr float NOVELTY_THRESHOLD = 0.5f;              // Novelty detection threshold
inline constexpr float NOVELTY_DECAY_RATE = 0.99f;            // Novelty decay per timestep

// Prediction error constants
inline constexpr float PREDICTION_ERROR_CLAMP = 0.5f;        // Prediction error clamp range
inline constexpr float PREDICTION_ERROR_LEARNING_RATE = 0.1f; // Prediction error learning rate

// ==============================================================================
// PREDICTION AND COGNITION CONSTANTS
// ==============================================================================

// Prediction system constants
inline constexpr float PREDICTION_ACCURACY_THRESHOLD = 0.9f;  // Confidence threshold
inline constexpr float PREDICTION_ERROR_DECAY = 0.95f;        // Prediction error decay

// Neural planning constants
inline constexpr int NEURAL_PLANNER_MAX_DEPTH = 5;            // Maximum planning depth
inline constexpr float PLANNING_EXPLORATION_FACTOR = 0.3f;    // Planning exploration factor

// Attention constants
inline constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f; // Attention inhibition
inline constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;  // Attention excitation
inline constexpr float ATTENTION_FOCUS_THRESHOLD = 0.7f;      // Attention focus threshold

// Concept formation constants
inline constexpr size_t CONCEPT_FORMATION_PATTERN_DIM = 20;   // Concept pattern dimension
inline constexpr float CONCEPT_SIMILARITY_THRESHOLD = 0.8f;   // Concept similarity threshold

// ==============================================================================
// PERFORMANCE CONSTANTS
// ==============================================================================

// Checkpoint system constants
inline constexpr size_t CHECKPOINT_BUFFER_SIZE = 10000;      // Checkpoint buffer size (KB)
inline constexpr float CHECKPOINT_COMPRESSION_LEVEL = 0.5f;   // Checkpoint compression (0-1)
inline constexpr int CHECKPOINT_SAVE_INTERVAL = 1000;       // Checkpoint save interval

// Memory pool constants
inline constexpr size_t MEMORY_POOL_ALIGNMENT = 64;          // Memory allocation alignment
inline constexpr size_t MEMORY_POOL_CHUNK_SIZE = 1024;       // Default memory chunk size

// Simulation performance constants
inline constexpr size_t SIMULATION_THREAD_POOL_SIZE = 4;     // Thread pool size for parallel processing
inline constexpr size_t MAX_SIMULATION_STEPS = 10000;        // Maximum simulation steps
inline constexpr double SIMULATION_TIME_LIMIT = 0.0;        // Simulation time limit (0 = unlimited)

// ==============================================================================
// SENSOR AND MOTOR CONSTANTS
// ==============================================================================

// Vision constants
inline constexpr size_t VISION_WIDTH_DEFAULT = 16;           // Default vision width
inline constexpr size_t VISION_HEIGHT_DEFAULT = 16;          // Default vision height
inline constexpr size_t VISION_CHANNELS_DEFAULT = 3;         // RGB channels
inline constexpr float VISION_SCALE_FACTOR = 5.0f;           // Vision intensity scaling

// Touch constants
inline constexpr size_t TOUCH_SENSORS_DEFAULT = 8;          // Default touch sensors
inline constexpr float TOUCH_COLLISION_THRESHOLD = 0.1f;     // Touch collision threshold

// Proprioception constants
inline constexpr size_t PROPRIOCEPTION_JOINTS_DEFAULT = 6; // Default joint sensors
inline constexpr float PROPRIOCEPTION_RANGE = 2.0f;          // Joint range (-1 to +1)

// Motor output constants
inline constexpr float MOTOR_ACTIVATION_THRESHOLD = 0.5f;    // Motor activation threshold
inline constexpr float MOTOR_MAX_EXPLORATION = 0.3f;         // Maximum exploration factor

// ==============================================================================
// CONFIG CONSTANTS
// ==============================================================================

// Default configuration values
inline constexpr size_t CONFIG_NEURON_COUNT = 500;            // Default neuron count
inline constexpr size_t CONFIG_REGION_COUNT = 1;             // Default region count
inline constexpr float CONFIG_CONNECTION_PROBABILITY = 0.15f; // Default connection probability
inline constexpr uint64_t CONFIG_RANDOM_SEED = 42;            // Default random seed
inline constexpr double CONFIG_SIMULATION_TIMESTEP = 0.001;  // Default timestep

// ==============================================================================
// TESTING AND DEBUGGING CONSTANTS
// ==============================================================================

// Learning experiment constants
inline constexpr float LEARNING_EXPERIMENT_WEIGHT_DELTA_THRESHOLD = 0.01f; // Weight change threshold
inline constexpr size_t LEARNING_EXPERIMENT_STEP_COUNT = 1000;           // Experiment step count

// STDP verification constants
inline constexpr float STDP_VERIFICATION_LTP_THRESHOLD = 0.001f; // LTP threshold
inline constexpr float STDP_VERIFICATION_LTD_THRESHOLD = -0.001f; // LTD threshold
inline constexpr int STDP_VERIFICATION_CORRELATED_PAIRS = 50;    // Correlated activity trials

// Performance benchmark constants
inline constexpr size_t PERFORMANCE_BENCHMARK_RUNS = 10;       // Benchmark repetitions
inline constexpr double PERFORMANCE_BENCHMARK_TIMEOUT = 60.0;   // Benchmark timeout (s)

// ==============================================================================
// MISCELLANEOUS CONSTANTS
// ==============================================================================

// Mathematical constants
inline constexpr float MATH_PI = 3.14159265358979323846f;
inline constexpr float MATH_EPSILON = 1e-6f;                   // Floating point epsilon
inline constexpr float MATH_INFINITY = std::numeric_limits<float>::infinity();

// Error and logging constants
inline constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024;       // Maximum log message length
inline constexpr size_t MAX_ERROR_STACK_DEPTH = 10;           // Maximum error stack trace

// ==============================================================================
// DEVELOPMENT PHASE CONSTANTS
// ==============================================================================

// Phase-specific constants
inline constexpr size_t PHASE1_NEURON_COUNT = 100;            // Phase 1 neuron count
inline constexpr size_t PHASE2_NEURON_COUNT = 500;            // Phase 2 neuron count  
inline constexpr size_t PHASE3_NEURON_COUNT = 1000;           // Phase 3 neuron count
inline constexpr size_t PHASE4_NEURON_COUNT = 2000;           // Phase 4 neuron count
inline constexpr size_t PHASE6_NEURON_COUNT = 10000;          // Phase 6 neuron count

// ==============================================================================

} // namespace nlm