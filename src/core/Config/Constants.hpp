#pragma once

namespace nlm {

// Configuration constants for NLM neural simulation framework
// These parameterize magic numbers and hard-coded values throughout the codebase
// to improve maintainability and configurability

// Vision processing constants
constexpr float VISION_INPUT_SCALE = 5.0f;      // Scale factor for vision intensity injection
constexpr float VISION_INPUT_CENTER = 1.0f;      // Center point for vision normalization
constexpr float VISION_INPUT_MIN = 0.0f;         // Minimum vision value (empty)
constexpr float VISION_INPUT_MAX = 1.0f;         // Maximum vision value (full)
constexpr size_t VISION_NEURON_COUNT = 16;       // Number of vision rays
constexpr size_t TEXT_VISION_OFFSET = 16;        // Offset for text neurons (tail vision)
constexpr size_t TEXT_VISION_COUNT = 15;         // Number of dedicated text neurons
constexpr float TEXT_NEURON_CURRENT = 8.0f;      // Current injected for text neurons

// Touch processing constants
constexpr float TOUCH_INPUT_SCALE = 8.0f;        // Scale factor for touch signals
constexpr float TOUCH_INPUT_CENTER = 0.0f;       // Center point for touch normalization
constexpr size_t TOUCH_NEURON_COUNT = 8;         // Number of touch sensory neurons
constexpr size_t TOUCH_FORWARD_SENSORS = 3;     // Touch sensors in forward cone (0, 1, 7)
constexpr float TOUCH_INTERACT_THRESHOLD = 0.05f; // Minimum touch to trigger interaction

// Internal signals constants
constexpr float INTERNAL_INPUT_SCALE = 2.0f;     // Scale factor for internal signals
constexpr float INTERNAL_INPUT_CENTER = 1.0f;    // Center point for internal normalization
constexpr float INTERNAL_INPUT_OFFSET = -1.0f;   // Offset for internal signals (2*x - 1)
constexpr size_t INTERNAL_NEURON_COUNT = 4;      // Number of internal sensory neurons

// Proprioception constants
constexpr float PROPRIOCEPTION_INPUT_SCALE = 3.0f; // Scale factor for proprioception
constexpr float PROPRIOCEPTION_INPUT_CENTER = 1.0f; // Center point for proprioception
constexpr float PROPRIOCEPTION_INPUT_OFFSET = -1.0f; // Offset for proprioception (2*x - 1)
constexpr size_t PROPRIOCEPTION_NEURON_COUNT = 6; // Number of proprioception neurons

// Motor neuron constants
constexpr size_t MOTOR_GROUP_COUNT = 6;           // Number of motor neuron groups (forward, backward, left, right, interact, wait)
constexpr float WTA_INJECTION_CURRENT = -4.0f;   // Current injected for WTA inhibition
constexpr float WINS_REQUIRED_FOR_INHIBITION = 3; // Consecutive wins before WTA inhibition applies
constexpr float WTA_INHIBITION_THRESHOLD = 5.0f;   // Firing rate threshold for decisive winner
constexpr float WTA_NOVELTY_THRESHOLD = 0.3f;       // Novelty level below which inhibition applies

// Curiosity and exploration constants
constexpr float CURIOSITY_EXPLORATION_CHANCE = 0.3f; // Maximum exploration probability
constexpr float CURIOSITY_WEIGHT_NOVELTY = 2.0f;     // Weight for novelty in curiosity calculation
constexpr float CURIOSITY_WEIGHT_PREDICTION_ERROR = 0.5f; // Weight for prediction error in curiosity
constexpr float CURIOSITY_MIN = 0.0f;                // Minimum curiosity level
constexpr float CURIOSITY_MAX = 1.0f;                // Maximum curiosity level
constexpr float CURIOSITY_STEADY_STATE_MOVING = 0.14f; // Steady curiosity during movement
constexpr float CURIOSITY_STEADY_STATE_STATIONARY = 0.0f; // Steady curiosity when stationary

// Prediction and novelty constants
constexpr float PREDICTION_ALPHA = 0.05f;          // Learning rate for prediction adaptation
constexpr float NOVELTY_DECAY_FACTOR = 0.99f;       // Decay factor for novelty signal
constexpr float NOVELTY_THRESHOLD = 0.02f;          // Minimum novelty to trigger behavior changes
constexpr float PREDICTION_ERROR_WEIGHT = 0.1f;    // Weight for prediction error in learning

// Neuromodulation constants
constexpr float DOPAMINE_CLAMP_MIN = -1.0f;        // Minimum dopamine level
constexpr float DOPAMINE_CLAMP_MAX = 1.0f;         // Maximum dopamine level
constexpr float PLACITY_FACTOR_BASE = 0.5f;        // Base plasticity factor
constexpr float PLACITY_FACTOR_RANGE = 1.0f;       // Range for plasticity factor adjustment
constexpr float LEARNING_RATE_REWARD_MODULATED = 0.05f; // Learning rate for reward-modulated plasticity
constexpr float REWARD_CLAMP_MIN = -1.0f;          // Minimum clamped reward
constexpr float REWARD_CLAMP_MAX = 1.0f;           // Maximum clamped reward

// Development stage constants
constexpr double DEVELOPMENT_INITIAL_END = 60.0;   // End of Initial stage (seconds)
constexpr double DEVELOPMENT_CRITICAL_END = 300.0; // End of CriticalPeriod stage (seconds)
constexpr double DEVELOPMENT_MATURATION_END = 900.0; // End of Maturation stage (seconds)
constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;   // High plasticity in initial stage
constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;  // Moderate plasticity in critical period
constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f; // Lower plasticity during maturation
constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;      // Low plasticity in adult stage

// Structural plasticity constants
constexpr float SYNAPTOGENESIS_RATE_INITIAL = 0.0001f;   // High synaptogenesis in early development
constexpr float SYNAPTOGENESIS_RATE_CRITICAL = 0.00008f;  // Reduced synaptogenesis
constexpr float SYNAPTOGENESIS_RATE_MATURATION = 0.000005f; // Minimal synaptogenesis
constexpr float PRUNING_RATE_INITIAL = 0.00001f;      // High pruning in early development
constexpr float PRUNING_RATE_CRITICAL = 0.000001f;     // Low pruning in critical period
constexpr float PRUNING_RATE_MATURATION = 1e-9f;      // Minimal pruning in maturation

// Concept formation constants
constexpr float CONCEPT_PATTERN_THRESHOLD = 0.01f;   // Minimum concept pattern magnitude
constexpr float CONCEPT_REWARD_THRESHOLD_POSITIVE = 0.1f; // Threshold for positive concepts
constexpr float CONCEPT_REWARD_THRESHOLD_NEGATIVE = -0.1f; // Threshold for negative concepts
constexpr float CONCEPT_BIAS_INTERACT_POSITIVE = 8.0f; // Bias toward interact for positive concepts
constexpr float CONCEPT_BIAS_INTERACT_NEGATIVE = -8.0f; // Bias away from interact for negative concepts
constexpr float CONCEPT_BIAS_TURN_AMOUNT = 6.0f;     // Turn bias for negative concepts

// Self-model constants
constexpr float SELF_MODEL_CONFIDENCE_THRESHOLD = 0.8f;   // Minimum confidence to trust self-model
constexpr float SELF_MODEL_CONFIDENCE_DIFFERENCE = 0.05f; // Minimum confidence difference to act
constexpr float SELF_MODEL_BIAS_AMOUNT = 3.0f;            // Amount of bias applied from self-model

// Social learning constants
constexpr float SOCIAL_SIMILARITY_THRESHOLD = 0.8f;   // Minimum similarity to trigger imitation
constexpr float SOCIAL_BIAS_AMOUNT = 5.0f;            // Amount of bias from social learning

// Winner-take-all (WTA) constants
constexpr float WTA_MARGIN_THRESHOLD = 5.0f;          // Margin required for decisive winner
constexpr float WTA_FIRING_RATE_THRESHOLD = 0.1f;     // Minimum firing rate to consider motor activity
constexpr float WTA_INTERACT_PROTECTION = 12.0f;      // Bonus for interact when it shouldn't win
constexpr float WTA_MOVING_FORWARD_BONUS = 10.0f;     // Bonus for moving forward with bright objects
constexpr float WTA_HAZARD_TURN_BONUS = 12.0f;       // Bonus for turning away from hazards
constexpr float WTA_RESOURCE_INTERACT_BONUS = 20.0f;  // Bonus for interacting with resources
constexpr float WTA_HAZARD_INTERACT_PENALTY = 12.0f; // Penalty for interacting with hazards
constexpr float WTA_TURN_DIVERSITY_BONUS = 6.0f;      // Bonus for turning when no turn for >25 steps
constexpr float WTA_REPETITION_PENALTY = -8.0f;      // Penalty for repeating same movement
constexpr float WTA_MOMENTUM_PENALTY = -8.0f;         // Penalty for reversing direction immediately

// Planning and curiosity constants
constexpr float PLANNER_OVERRIDE_THRESHOLD = 0.15f;   // Quality gap required for planner override
constexpr float PLANNER_LEARNING_THRESHOLD = 0.05f;   // Minimum action quality for learning
constexpr float PLANNER_ACTION_QUALITY_SCALE = 0.1f;  // Scale factor for action quality

// Neural dynamics constants
constexpr float LIF_TAU_MEMBRANE = 20.0f;            // Membrane time constant
constexpr float LIF_THRESHOLD_ADJUSTMENT = 0.01f;   // Threshold for threshold adjustment
constexpr float LIF_SPIKE_AMPLITUDE = 1.0f;          // Amplitude of spike event
constexpr float LIF_REFACTORY_RECOVER_TIME = 9.0f;   // Refractory period length (steps)
constexpr float LIF_FREQUENCY_SCALE = 1000.0f;       // Scale factor for firing rate calculation

// Memory system constants
constexpr size_t WORKING_MEMORY_CAPACITY = 100;     // Working memory capacity
constexpr size_t EPISODIC_MEMORY_CAPACITY = 1000;   // Episodic memory capacity
constexpr size_t ASSOCIATIVE_MEMORY_CAPACITY = 500; // Associative memory capacity
constexpr float MEMORY_COMPETITION_STRENGTH = 0.9f;  // Strength of memory competition
constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.5f; // Threshold for memory consolidation

// Error handling constants
constexpr float ERROR_VALUE_MIN = -1.0f;             // Minimum valid error value
constexpr float ERROR_VALUE_MAX = 1.0f;              // Maximum valid error value
constexpr float ERROR_CLAMP_FACTOR = 1.0f;           // Factor for clamping error values
constexpr float TOLERANCE_EPSILON = 1e-6f;            // Small tolerance for floating point comparisons

// Performance constants
constexpr size_t MAX_THREADS = 4;                   // Maximum number of threads for parallel processing
constexpr size_t SIMD_VECTOR_SIZE = 4;               // SIMD vector size (float elements)
constexpr size_t CACHE_LINE_SIZE = 64;               // Cache line size in bytes
constexpr double SIMULATION_STEP_DEFAULT = 0.001;   // Default simulation timestep
constexpr size_t SIMULATION_MAX_STEPS = 1000000;    // Maximum simulation steps

// Logging and debugging constants
constexpr int LOG_LEVEL_TRACE = 0;                   // Trace level logging
constexpr int LOG_LEVEL_DEBUG = 1;                   // Debug level logging
constexpr int LOG_LEVEL_INFO = 2;                    // Info level logging
constexpr int LOG_LEVEL_WARNING = 3;                // Warning level logging
constexpr int LOG_LEVEL_ERROR = 4;                   // Error level logging
constexpr int LOG_LEVEL_FATAL = 5;                   // Fatal level logging
constexpr size_t LOG_MAX_MESSAGE_LENGTH = 4096;     // Maximum log message length
constexpr double LOG_FLUSH_INTERVAL = 0.1;           // Log flush interval (seconds)

} // namespace nlm
