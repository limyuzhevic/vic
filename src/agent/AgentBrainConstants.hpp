// AgentBrain-specific constants
#pragma once

namespace nlm {

// Sensory input scaling constants
static constexpr float VISION_INPUT_SCALE = 5.0f;
static constexpr float TOUCH_INPUT_SCALE = 8.0f;
static constexpr float INTERNAL_INPUT_SCALE = 5.0f;
static constexpr float PROPRIOCEPTION_INPUT_SCALE = 3.0f;

// Novelty detection constants
static constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;

// Motor command distribution constants
static constexpr size_t MOTOR_ACTION_GROUPS = 6;
static constexpr size_t SENSORY_CHANNEL_GROUPS = 4;

// Activity threshold constants
static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
static constexpr float MIN_MEANINGFUL_ACTIVITY = 0.5f;

// Neuromodulation thresholds
static constexpr float CURIOSITY_THRESHOLD_FOR_EXPLORATION = 0.3f;
static constexpr float CURIOSITY_THRESHOLD_FOR_RANDOM_EXPLORATION = 0.5f;
static constexpr float MAX_EXPLORATION_CHANCE = 0.3f;  // 30% max random chance

// Reward modulation constants
static constexpr float REWARD_PREDICTION_WEIGHT = 0.95f;
static constexpr float REWARD_ERROR_WEIGHT = 0.05f;
static constexpr float DEFAULT_DOPAMINE_CLAMP_MIN = -1.0f;
static constexpr float DEFAULT_DOPAMINE_CLAMP_MAX = 1.0f;

// Development constants
static constexpr double DEVELOPMENT_STAGE_1_DURATION = 60.0;   // Initial stage
static constexpr double DEVELOPMENT_STAGE_2_DURATION = 240.0;  // Critical period  (60 + 180)
static constexpr double DEVELOPMENT_STAGE_3_DURATION = 600.0;  // Maturation (300 + 300)

// Neural activation thresholds
static constexpr float NEURON_FIRING_ACTIVITY_THRESHOLD = 5.0f;
static constexpr float NEURON_ACTIVATION_NORMALIZATION_DIVISOR = 20.0f;

} // namespace nlm