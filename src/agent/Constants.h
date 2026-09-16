#ifndef NLM_CONSTANTS_H
#define NLM_CONSTANTS_H

// NLM Constants Header
// Centralized constants for improved maintainability and configurability

// Motor decoder constants
static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
static constexpr float MOTOR_CURIOUSITY_THRESHOLD = 0.3f;
static constexpr float MOTOR_EXPLORATION_CHANCE_FACTOR = 0.3f;
static constexpr float MOTOR_EXPLORATION_CURiosity_THRESHOLD = 0.5f;
static constexpr size_t MOTOR_RANDOM_ACTION_RANGE = 7;

// Sensory input scaling constants
static constexpr float VISION_CURRENT_SCALE = 5.0f;
static constexpr float TOUCH_CURRENT_SCALE = 8.0f;
static constexpr float INTERNAL_CURRENT_SCALE = 5.0f;
static constexpr float INTERNAL_CURRENT_CENTER = 1.0f;
static constexpr float PROPRIOCEPTION_CURRENT_SCALE = 3.0f;

// Neuromodulation constants
static constexpr float NEUROMODULATION_CLAMP_MIN = -1.0f;
static constexpr float NEUROMODULATION_CLAMP_MAX = 1.0f;
static constexpr float NEUROMODULATION_WEIGHT_CHANGE_CLIP = 0.001f;
static constexpr float NEUROMODULATION_PLATYSITY_FACTOR_MIN = 0.1f;
static constexpr float NEUROMODULATION_PLATYSITY_FACTOR_MAX = 2.0f;

// Development constants
static constexpr double DEVELOPMENT_STAGE_1_DURATION = 60.0;
static constexpr double DEVELOPMENT_STAGE_2_DURATION = 300.0; // 60 + 240
static constexpr double DEVELOPMENT_STAGE_3_DURATION = 900.0; // 300 + 600
static constexpr float INITIAL_PLASTICITY = 1.0f;
static constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;
static constexpr float MATURATION_PLASTICITY = 0.5f;
static constexpr float ADULT_PLASTICITY = 0.2f;

// Memory system constants
static constexpr float NOVELTY_DECAY_RATE = 0.99f;
static constexpr float NOVELTY_MULTIPLIER = 2.0f;
static constexpr float PREDICTION_ERROR_MULTIPLIER = 0.5f;
static constexpr float MAX_NOVELTY_LEVEL = 1.0f;

// Reward modulation constants
static constexpr float EXPECTED_REWARD_SMOOTHING = 0.95f;
static constexpr float EXPECTED_REWARD_LEARNING_RATE = 0.05f;
static constexpr float DOPAMINE_PREDICTION_ERROR_SCALE = 1.0f;
static constexpr float PLASTICITY_MODULATION_BASE = 0.5f;
static constexpr float PLASTICITY_MODULATION_AMPLITUDE = 0.5f;

// Structural plasticity constants
static constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
static constexpr float PRUNING_BASE_RATE = 0.00001f;
static constexpr float PLASTICITY_MODIFIER_RANGE = 2.0f;

#endif // NLM_CONSTANTS_H