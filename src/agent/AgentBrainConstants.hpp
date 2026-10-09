#pragma once

namespace nlm {

namespace Constants {
    // Motor control constants
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CURiosity_THRESHOLD = 0.5f;
    constexpr float NOVELTY_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float CURIOSITY_MAX_EXPLORATION_CHANCE = 0.3f;
    
    // Neuromodulation constants
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float PLasticity_MODIFIER_BASE = 1.0f;
    constexpr float PREDICTED_REWARD_DECAY_RATE = 0.95f;
    constexpr float REWARD_LEARNING_RATE = 0.05f;
    
    // Development constants
    constexpr double DEVELOPMENT_STAGE_INITIAL_MAX_AGE = 60.0;    // seconds
    constexpr double DEVELOPMENT_STAGE_CRITICAL_MAX_AGE = 300.0;  // seconds
    constexpr double DEVELOPMENT_STAGE_MATURATION_MAX_AGE = 900.0; // seconds
    constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;
    constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;
    constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;
    constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;
    
    // Sensory processing constants
    constexpr float VISION_SENSORY_SCALE = 5.0f;
    constexpr float TOUCH_SENSORY_SCALE = 8.0f;
    constexpr float INTERNAL_SENSORY_SCALE = 5.0f;
    constexpr float PROPRIOCEPTION_SENSORY_SCALE = 3.0f;
    constexpr float SENSORY_NOVELTY_DECAY = 0.99f;
    constexpr float NOVELTY_NORMALIZATION_FACTOR = 1.0f;
    constexpr float CURIOSITY_GAIN_NOVELTY = 2.0f;
    constexpr float CURIOSITY_GAIN_PREDICTION_ERROR = 0.5f;
    
    // Motor neuron distribution
    constexpr size_t MOTOR_GROUP_COUNT = 6; // forward, backward, turn left, turn right, interact, wait
    constexpr size_t SENSORY_GROUP_COUNT = 4; // vision, touch, internal, proprioception
    
    // Simulation constants
    constexpr size_t MAX_MOTOR_COMMAND_RANDOM_CHOICE = 7; // choices for exploration
    
    // Eligibility trace constants
    constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;
    
    // Structural plasticity constants
    constexpr float SYNAPTOGENESIS_RATE_BASE = 0.0001f;
    constexpr float PRUNING_RATE_BASE = 0.00001f;
    constexpr float PLASTICITY_SYNTHESIS_FACTOR = 1.0f;
    constexpr float PLASTICITY_PRUNE_FACTOR = 2.0f;
    
    // Development stages
    constexpr float INITIAL_PLASTICITY = 1.0f;
    constexpr float ADULT_PLASTICITY = 0.2f;
    
} // namespace Constants

} // namespace nlm