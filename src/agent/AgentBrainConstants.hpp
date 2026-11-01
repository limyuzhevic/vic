// AgentBrain Constants and Configuration
// This file defines all magic numbers and configuration constants
// to improve code readability and maintainability

#pragma once

namespace nlm {
namespace AgentBrainConstants {

// ============= DIMENSIONS AND SIZES =============

// Vision dimensions (16x16 grid)
constexpr size_t VISION_WIDTH = 16;
constexpr size_t VISION_HEIGHT = 16;
constexpr size_t VISION_TOTAL = VISION_WIDTH * VISION_HEIGHT;

// Touch/proximity sensor counts
constexpr size_t TOUCH_SENSOR_COUNT = 8;

// Internal state signal counts
constexpr size_t INTERNAL_SIGNAL_COUNT = 4;

// Proprioception signal counts
constexpr size_t PROPRIOCEPTION_SENSOR_COUNT = 6;

// Audio signal counts
constexpr size_t AUDIO_SIGNAL_COUNT = 0; // Currently disabled

// Motor action counts (including Wait)
constexpr size_t MOTOR_ACTION_COUNT = 6;
constexpr size_t MOTOR_GROUP_COUNT = 5; // Excluding Wait group

// Sensory neuron group counts
constexpr size_t SENSORY_VISION_GROUP_COUNT = VISION_TOTAL;
constexpr size_t SENSORY_TOUCH_GROUP_COUNT = TOUCH_SENSOR_COUNT;
constexpr size_t SENSORY_INTERNAL_GROUP_COUNT = INTERNAL_SIGNAL_COUNT;
constexpr size_t SENSORY_PROPGRIOCEPTION_GROUP_COUNT = PROPRIOCEPTION_SENSOR_COUNT;

// Text encoding (Phase 6: text-as-a-sense)
constexpr size_t TEXT_ENCODING_OUTPUT_SIZE = 4; // Number of neurons encoded per word
constexpr size_t TEXT_DEDICATED_VISION_OFFSET = 16; // Text neurons start at 16
constexpr size_t TEXT_DEDICATED_VISION_COUNT = 15; // Neurons 16-30 for text

// ============= THRESHOLDS AND LIMITS =============

// Touch/proximity thresholds
constexpr float TOUCH_RANGE_THRESHOLD = 0.05f; // >0.05 means something is within touch range
constexpr float TOUCH_STRENGTH_THRESHOLD = 0.05f; // Same as above

// Vision/object thresholds
constexpr float VISION_EMPTY_THRESHOLD = 0.99f; // Values >=0.99 are considered "empty"
constexpr float VISION_RESOURCE_THRESHOLD = 0.5f; // >0.5 and <0.99 = resource cue
constexpr float VISION_HAZARD_THRESHOLD = 0.35f; // <0.35 = hazard/wall cue
constexpr float VISION_BRIGHT_APPROACH_THRESHOLD = 0.3f; // >0.3 = phototaxis
constexpr float VISION_BRIGHT_APPROACH_MAX = 0.95f; // <0.95 to prevent wall collision

// Novelty and curiosity thresholds
constexpr float NOVELTY_SENSITIVITY_THRESHOLD = 0.02f; // Novelty must exceed this for reflex bonus
constexpr float NOVELTY_ADAPTATION_ALPHA = 0.05f; // Prediction adaptation rate (20-step lag)
constexpr float NOVELTY_DECAY_FACTOR = 0.99f; // Novelt decay per step
constexpr float CURIOSITY_NOVELTY_COEFFICIENT = 2.0f; // Curiosity multiplier
constexpr float CURIOSITY_PREDICTION_ERROR_COEFFICIENT = 0.5f; // Curiosity multiplier
constexpr float CURIOSITY_TRACE_COEFFICIENT = 0.6f; // Curiosity trace rise
constexpr float CURIOSITY_MAX_LEVEL = 1.0f; // Maximum curiosity level

// Planner and concept thresholds
constexpr float PLANNER_ACTION_QUALITY_THRESHOLD = 0.02f; // Quality beyond this means "learned"
constexpr float PLANNER_QUALITY_GAP_THRESHOLD = 0.15f; // Quality gap needed for bias
constexpr float CONCEPT_PATTERN_THRESHOLD = 0.01f; // Pattern norm threshold
constexpr float CONCEPT_REWARD_POSITIVE_THRESHOLD = 0.1f; // >0.1 = positive resource concept
constexpr float CONCEPT_REWARD_NEGATIVE_THRESHOLD = -0.1f; // <-0.1 = negative hazard concept
constexpr float CONCEPT_BIAS_STRENGTH = 8.0f; // Bias strength for positive concepts
constexpr float CONCEPT_PENALTY_STRENGTH = 8.0f; // Penalty strength for negative concepts

// Self-model and social thresholds
constexpr float SELF_MODEL_CONFIDENCE_THRESHOLD = 0.8f; // High confidence
constexpr float SELF_MODEL_CONSISTENCY_THRESHOLD = 0.05f; // Clear preference
constexpr float SOCIAL_SIMILARITY_THRESHOLD = 0.8f; // Social imitation threshold
constexpr float SOCIAL_BIAS_STRENGTH = 5.0f; // Social bias strength

// Winner-take-all (WTA) thresholds
constexpr float WTA_DECISION_THRESHOLD = 5.0f; // Margin needed for WTA pulse
constexpr float WTA_CONSECUTIVE_WINNER_LIMIT = 3; // Max consecutive same winners
constexpr float WTA_REFLEX_INTERACT_EXCLUSION = 4; // Interact group excluded from WTA pulse

// Turn diversity and fatigue thresholds
constexpr size_t STEPS_SINCE_TURN_THRESHOLD = 25; // Steps without turning
constexpr size_t REPEAT_ACTION_THRESHOLD = 3; // Repetitions before fatigue penalty
constexpr float TURN_DIVERSITY_BONUS = 6.0f; // Bonus for turning after long inaction
constexpr float REPEAT_PENALTY = 8.0f; // Penalty for repeated actions

// Momentum and reversal thresholds
constexpr float MOMENTUM_REVERSAL_PENALTY = 8.0f; // Penalty for reversing direction
constexpr float MOTOR_SILENCE_THRESHOLD = 0.1f; // Below this = silent brain (Wait only)

// Interaction and reflex thresholds
constexpr float REFLEX_REWARD_THRESHOLD = 0.01f; // Reward magnitude for reflex learning
constexpr float REFLEX_INTERACT_DURATION = 1.0f; // Duration of interact reflex bonus
constexpr float REFLEX_INTERACT_SUPPRESSION = 12.0f; // Suppression strength for hazard

// ============= BIAS AND MODULATION STRENGTHS =============

// Motor command bias strengths
constexpr float VISION_GATED_INTERACT_BONUS = 20.0f; // Vision-gated interact reflex
constexpr float HAZARD_SUPPRESSION_BONUS = 12.0f; // Hazard turn away bonus
constexpr float BRIGHT_APPROACH_BONUS = 10.0f; // Phototaxis bonus
constexpr float PHOTOTAXIS_BONUS = 10.0f; // Bright object approach bonus (Phase 6)

// Planner and concept bias strengths
constexpr float PLANNER_BIAS_STRENGTH = 15.0f; // Planner bias strength
constexpr float CONCEPT_POSITIVE_BIAS = 8.0f; // Positive concept bias
constexpr float CONCEPT_NEGATIVE_PENALTY = 8.0f; // Negative concept penalty
constexpr float CONCEPT_TURN_BONUS = 6.0f; // Turn bonus for negative concepts
constexpr float SELF_BIAS_STRENGTH = 3.0f; // Self-model bias strength

// Neuromodulation coefficients
constexpr float DOPAMINE_LEARNING_RATE = 0.05f; // Dopamine learning rate
constexpr float DOPAMINE_CLAMP_MIN = -1.0f; // Dopamine clamp minimum
constexpr float DOPAMINE_CLAMP_MAX = 1.0f; // Dopamine clamp maximum
constexpr float PLASTICITY_MODIFIER_FACTOR = 0.5f; // Plasticity modulation factor

// ============= TIME AND DEVELOPMENT =============

// Development stages
constexpr double DEVELOPMENT_INITIAL_STAGE = 60.0; // Initial development stage duration
constexpr double DEVELOPMENT_CRITICAL_PERIOD = 300.0; // Critical period duration
constexpr double DEVELOPMENT_MATURATION_STAGE = 900.0; // Maturation stage duration

// Refractory periods
constexpr uint32_t NEURON_REFACTORY_STEPS = 9; // Refractory steps (Phase 6)
constexpr uint32_t NEURON_REFACTORY_STEPS_OLD = 5; // Old refractory steps (Phase 4)

// Time constants
constexpr double SIMULATION_TIMESTEP_DEFAULT = 0.001; // Default simulation timestep
constexpr double SIMULATION_CLOCK_RESOLUTION = 0.001; // Simulation clock resolution

// ============= MEMORY AND STATE =============

// State persistence
constexpr float STATE_PERSISTENCE_THRESHOLD = 0.001f; // Eligibility trace persistence threshold
constexpr float STATE_DECAY_RATE = 0.1f; // State decay rate

// Memory and pattern matching
constexpr size_t PATTERN_VECTOR_SIZE = VISION_TOTAL + TOUCH_SENSOR_COUNT + INTERNAL_SIGNAL_COUNT; // For concept patterns
constexpr float PATTERN_MATCHING_THRESHOLD = 0.5f; // Pattern matching threshold

// ============= MISC CONSTANTS =============

// Random seed and initialization
constexpr uint64_t DEFAULT_RANDOM_SEED = 42; // Default random seed
constexpr uint64_t TIME_BASED_SEED_MIN = 1000; // Minimum time-based seed
constexpr uint64_t TIME_BASED_SEED_MAX = 999999999; // Maximum time-based seed

// Logging and debugging
constexpr size_t MAX_THOUGHT_TRACE_LENGTH = 256; // Maximum thought trace length
constexpr size_t MAX_LOG_MESSAGE_LENGTH = 1024; // Maximum log message length

// Safety and validation
constexpr float EPSILON_FLOAT = 0.0001f; // Small epsilon for float comparisons
constexpr float SAFE_FLOAT_RANGE_MIN = -1000.0f; // Safe minimum float value
constexpr float SAFE_FLOAT_RANGE_MAX = 1000.0f; // Safe maximum float value

} // namespace AgentBrainConstants
} // namespace nlm