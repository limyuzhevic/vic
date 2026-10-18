// Configuration constants for NLM AgentBrain
// These constants can be modified in config files if needed
#pragma once

namespace nlm {

namespace AgentBrainConstants {
    // Motor neuron distribution groups
    constexpr size_t MOTOR_GROUPS_COUNT = 6;
    constexpr size_t SENSORY_GROUPS_COUNT = 4;
    
    // Default motor neuron group indices
    enum MotorGroup {
        MOTOR_FORWARD = 0,
        MOTOR_BACKWARD = 1,
        MOTOR_TURN_LEFT = 2,
        MOTOR_TURN_RIGHT = 3,
        MOTOR_INTERACT = 4,
        MOTOR_WAIT = 5
    };
    
    // Default sensory neuron group indices
    enum SensoryGroup {
        SENSORY_VISION = 0,
        SENSORY_TOUCH = 1,
        SENSORY_INTERNAL = 2,
        SENSORY_PROPRIOCEPTION = 3
    };
    
    // Motor neuron scaling factors
    constexpr float MOTOR_FORWARD_SCALE = 5.0f;
    constexpr float MOTOR_BACKWARD_SCALE = 5.0f;
    constexpr float MOTOR_TURN_LEFT_SCALE = 5.0f;
    constexpr float MOTOR_TURN_RIGHT_SCALE = 5.0f;
    constexpr float MOTOR_INTERACT_SCALE = 8.0f;
    constexpr float MOTOR_WAIT_SCALE = 8.0f;
    
    // Sensory neuron scaling factors
    constexpr float SENSORY_VISION_SCALE = 5.0f;
    constexpr float SENSORY_TOUCH_SCALE = 8.0f;
    constexpr float SENSORY_INTERNAL_SCALE = 5.0f;
    constexpr float SENSORY_PROPRIOCEPTION_SCALE = 3.0f;
    
    // Curiosity and exploration thresholds
    constexpr float CURIOSITY_THRESHOLD = 0.3f;
    constexpr float HIGH_CURIOSITY_THRESHOLD = 0.5f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f; // 30% max random exploration
    
    // Neuromodulation parameters
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float PLASTICITY_FACTOR_BASE = 0.5f;
    constexpr float PLASTICITY_FACTOR_STDEV = 0.5f;
    
    // Development stages
    constexpr double DEVELOPMENT_INITIAL = 60.0;  // ~1 minute
    constexpr double DEVELOPMENT_CRITICAL = 300.0;  // ~5 minutes
    constexpr double DEVELOPMENT_MATURATION = 900.0;  // ~15 minutes
    constexpr float PLASTICITY_INITIAL = 1.0f;
    constexpr float PLASTICITY_CRITICAL = 0.8f;
    constexpr float PLASTICITY_MATURATION = 0.5f;
    constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Structural plasticity rates
    constexpr float SYNAPTOGENESIS_RATE_INITIAL = 0.0001f;
    constexpr float PRUNING_RATE_INITIAL = 0.00001f;
    
    // Novelty detection
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;
    constexpr float CURIOUSITY_NOVELTY_WEIGHT = 2.0f;
    constexpr float CURIOUSITY_ERROR_WEIGHT = 0.5f;
    constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
    
    // Motor command selection
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_WAIT_ACTIVITY = 0.0f;
    
    // Random exploration
    constexpr size_t MAX_RANDOM_MOTOR_COMMAND = 7;  // 0-7
    
    // STDP parameters
    constexpr float STDP_DEFAULT_LTP = 0.02f;
    constexpr float STDP_DEFAULT_LTD = 0.015f;
    constexpr float STDP_DEFAULT_TAU = 20.0f;
    
    // Configuration validation
    constexpr size_t MIN_VISION_SIZE = 1;
    constexpr size_t MIN_TOUCH_SIZE = 1;
    constexpr size_t MIN_INTERNAL_SIZE = 1;
    constexpr size_t MIN_PROPRIOCEPTION_SIZE = 1;
} // namespace AgentBrainConstants

} // namespace nlm
