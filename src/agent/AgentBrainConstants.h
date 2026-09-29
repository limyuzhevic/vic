#pragma once

namespace nlm {

// AgentBrain scaling factors and magic numbers
// These should be made configurable in the future
class AgentBrainConstants {
public:
    // Motor neuron distribution
    static constexpr size_t MOTOR_GROUP_COUNT = 6;
    
    // Sensory neuron distribution  
    static constexpr size_t SENSORY_GROUP_COUNT = 4;
    
    // Activity thresholds
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float CURIOSITY_THRESHOLD = 0.3f;
    static constexpr float EXPLORATION_THRESHOLD = 0.5f;
    static constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    static constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    
    // Plasticity modifiers
    static constexpr float PLASTICITY_INITIAL = 1.0f;
    static constexpr float PLASTICITY_CRITICAL_PERIOD = 0.8f;
    static constexpr float PLASTICITY_MMATURATION = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Developmental stages
    static constexpr double DEVELOPMENT_INITIAL = 0.0;
    static constexpr double DEVELOPMENT_CRITICAL = 60.0;
    static constexpr double DEVELOPMENT_MATURATION = 300.0;
    static constexpr double DEVELOPMENT_ADULT = 900.0;
    
    // Novelty detection
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    
    // Motor decoding
    static constexpr float MOTOR_VISION_SCALE = 5.0f;
    static constexpr float MOTOR_TOUCH_SCALE = 8.0f;
    static constexpr float MOTOR_INTERNAL_SCALE = 5.0f;
    static constexpr float MOTOR_PROPRIOCEPTION_SCALE = 3.0f;
    
    // Curiosiy calculation
    static constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;
    static constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f;
    
    // Reward modulation
    static constexpr float REWARD_EXCLUDECTED_REWARD_DECAY = 0.95f;
    static constexpr float REWARD_ELIGIBILITY_DECAY = 0.1f;
    
    // Structural plasticity rates
    static constexpr float SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float PRUNING_RATE = 0.00001f;
};

} // namespace nlm