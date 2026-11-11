#pragma once

#include "../core/Constants.hpp"

namespace nlm {

// Neural population parameters
constexpr size_t VISION_NEURON_COUNT = 31;          // Vision group neurons (16 rays + 15 text neurons)
constexpr size_t TOUCH_NEURON_COUNT = 8;            // Touch/proximity sensors
constexpr size_t INTERNAL_NEURON_COUNT = 4;        // Internal state neurons
constexpr size_t PROPRIOCEPTION_NEURON_COUNT = 6;   // Body position sensors
constexpr size_t MOTOR_NEURON_COUNT = 6;            // Motor neurons (one per action type)

// Population distribution parameters
constexpr size_t MAX_DISTRIBUTION_ATTEMPTS = 100;   // Maximum attempts for neuron distribution
constexpr float NEURON_DISTRIBUTION_EPSILON = 0.001f; // Small value for numerical stability

// Sensory encoding parameters
constexpr float VISION_CURRENT_SCALE_FACTOR = 5.0f;     // Scale vision intensity to current (1.0 vision = 5.0 pA)
constexpr float TOUCH_CURRENT_SCALE_FACTOR = 8.0f;     // Collision signal scaling
constexpr float INTERNAL_CURRENT_SCALE_FACTOR = 2.5f;   // Internal signal center and scale
constexpr float PROPRIOCEPTION_CURRENT_SCALE_FACTOR = 3.0f; // Proprioception scaling
constexpr float TEXT_CURRENT_STRENGTH = 8.0f;           // Text-as-a-sense injection strength

// Novelty detection parameters
constexpr float NOVELTY_PREDICTION_ALPHA = 0.05f;        // Prediction adaptation rate (alpha in exponential moving average)
constexpr float NOVELTY_SENSITIVITY = 2.0f;              // Novelty multiplier (novelty = diff * sensitivity)
constexpr float SLOW_PREDICTION_THRESHOLD = 0.3f;       // Threshold for prediction-based novelty detection
constexpr float SENSORY_NOVELTY_DECAY = 0.99f;          // Decay factor for curiosity trace

// Curiosity and exploration parameters
constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;         // Weight of novelty in curiosity calculation
constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f; // Weight of prediction error in curiosity
constexpr float CURIOSITY_TRACE_INCREMENT = 0.6f;       // Increment for curiosity trace (empirically determined)
constexpr float CURIOSITY_THRESHOLD = 0.15f;            // Minimum curiosity level for exploration
constexpr float CURIOSITY_CLAMP_MAX = 1.0f;             // Maximum curiosity level

// Winner-take-all lateral inhibition parameters
constexpr float WTA_MARGIN_THRESHOLD = 5.0f;            // Margin above runner-up to trigger inhibition
constexpr float WTA_NOVELTY_THRESHOLD = 0.3f;           // Novelty level below which inhibition applies
constexpr float WTA_CONSECUTIVE_WINS_LIMIT = 3;         // Maximum consecutive wins before inhibition resets
constexpr float WTA_INHIBITION_STRENGTH = -4.0f;        // Inhibition current strength per suppressed neuron
constexpr size_t WTA_INHIBITION_GROUP_SIZE = 5;        // Number of motor groups competing in WTA

// Reflex and bias parameters
constexpr float PROXIMITY_REFLEX_THRESHOLD = 0.05f;     // Touch range detection threshold
constexpr float PROXIMITY_REFLEX_STRENGTH = 12.0f;       // Interact bonus strength when something is nearby
constexpr float HAZARD_TURN_BOOST = 12.0f;              // Turn away from hazards (positive bias)
constexpr float PHOTOTAXIS_BONUS = 10.0f;               // Bright object approach bias
constexpr float BIAS_MAX_EFFECT = 5.0f;                  // Maximum bias effect (from learned preference)

// Planner bias parameters
constexpr float PLANNER_LEARNED_THRESHOLD = 0.02f;     // Minimum action quality to be considered learned
constexpr float PLANNER_CONFLICT_THRESHOLD = 0.05f;    // Quality difference needed for planner override
constexpr float PLANNER_BONUS_STRENGTH = 15.0f;         // Bonus strength when planner overrides motor
constexpr float PLANNER_EXPLORATION_THRESHOLD = 0.15f; // Quality gap needed for planner suggestion

// Self-model and social learning parameters
constexpr size_t MAX_SELF_MODEL_ENTRIES = 6;            // Number of action types for self-model
constexpr size_t MAX_SOCIAL_OBSERVATIONS = 1000;       // Maximum social learning observations
constexpr float SELF_MODEL_CONFIDENCE_THRESHOLD = 0.05f; // Minimum confidence to influence decisions

// Development system parameters
constexpr double DEVELOPMENT_INITIAL_AGE = 0.0;         // Starting developmental age
constexpr double DEVELOPMENT_CRITICAL_START = 100.0;  // Start of critical development period
constexpr double DEVELOPMENT_CRITICAL_END = 500.0;    // End of critical development period
constexpr double DEVELOPMENT_MATURATION_START = 500.0; // Start of maturation period
constexpr double DEVELOPMENT_MATURATION_END = 1000.0; // End of maturation period
constexpr float DEVELOPMENT_INITIAL_STAGE = 1.0f;     // Plasticity modifier during initial stage
constexpr float DEVELOPMENT_CRITICAL_STAGE = 2.0f;   // Plasticity modifier during critical stage
constexpr float DEVELOPMENT_MATURATION_STAGE = 1.5f;  // Plasticity modifier during maturation
constexpr float DEVELOPMENT_ADULT_STAGE = 1.0f;       // Plasticity modifier in adult stage

// Reward modulation parameters
constexpr float REWARD_PREDICTION_ERROR_SCALE = 1.0f;  // Scaling factor for prediction error
constexpr float REWARD_ADAPTATION_RATE = 0.1f;         // Rate of reward expectation adaptation
constexpr float REWARD_NOISE_LEVEL = 0.01f;            // Noise added to reward signals

// Configuration system parameters
constexpr uint64_t MAX_CONFIG_ENTRIES = 1000;          // Maximum configuration entries
constexpr size_t MAX_CONFIG_LINE_LENGTH = 1024;        // Maximum line length in config files
constexpr size_t MAX_JSON_DEPTH = 10;                  // Maximum JSON object nesting depth

// Logging and debugging parameters
constexpr size_t MAX_LOG_BUFFER_SIZE = 10000;          // Maximum log buffer size
constexpr float LOG_FLUSH_INTERVAL = 0.1f;             // Log flush interval in seconds
constexpr size_t MAX_STACK_TRACE_DEPTH = 50;          // Maximum stack trace depth for debugging

// Performance optimization parameters
constexpr size_t NEURON_BATCH_SIZE = 100;              // Batch size for neuron operations
constexpr size_t SYNAPSE_BATCH_SIZE = 50;              // Batch size for synapse operations
constexpr size_t REGION_BATCH_SIZE = 20;              // Batch size for region operations

// Debugging and validation parameters
constexpr float VALIDATION_EPSILON = 0.0001f;          // Small value for floating-point comparisons
constexpr size_t VALIDATION_MAX_CONFLICTS = 100;       // Maximum validation conflicts before error
constexpr float SAFETY_CHECK_BOUNDARY = 1000.0f;      // Boundary for safety checks

} // namespace nlm