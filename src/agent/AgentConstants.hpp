#pragma once

namespace nlm {

// Neural configuration constants
struct AgentConstants {
    // Sensory neuron group sizes
    static constexpr size_t VISION_NEURONS = 256;
    static constexpr size_t TOUCH_NEURONS = 8;
    static constexpr size_t INTERNAL_NEURONS = 4;
    static constexpr size_t PROPRIOCEPTION_NEURONS = 6;
    
    // Total sensory neurons
    static constexpr size_t TOTAL_SENSORY_NEURONS = VISION_NEURONS + TOUCH_NEURONS + 
                                                   INTERNAL_NEURONS + PROPRIOCEPTION_NEURONS;
    
    // Motor neuron groups (6 actions from MotorCommand enum)
    static constexpr size_t MOTOR_FORWARD = 0;
    static constexpr size_t MOTOR_BACKWARD = 1;
    static constexpr size_t MOTOR_TURN_LEFT = 2;
    static constexpr size_t MOTOR_TURN_RIGHT = 3;
    static constexpr size_t MOTOR_INTERACT = 4;
    static constexpr size_t MOTOR_WAIT = 5;
    static constexpr size_t MOTOR_LOOK_LEFT = 6;
    static constexpr size_t MOTOR_LOOK_RIGHT = 7;
    static constexpr size_t TOTAL_MOTOR_GROUPS = 8; // Including look left/right
    
    // Motor neuron distribution
    static constexpr size_t MOTOR_GROUPS_PER_CATEGORY = 6;  // Forward, backward, left, right, interact, wait
    static constexpr size_t ADDITIONAL_MOTOR_ACTIONS = 2;   // Look left, look right
    
    // Scaling factors for sensory transduction
    static constexpr float VISION_SCALE = 5.0f;
    static constexpr float TOUCH_SCALE = 8.0f;
    static constexpr float INTERNAL_SCALE = 5.0f;
    static constexpr float PROPRIOCEPTION_SCALE = 3.0f;
    
    // Neuromodulation parameters
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float NOVELTY_THRESHOLD = 0.1f;
    static constexpr float CURIOUSITY_THRESHOLD = 0.3f;
    static constexpr float MAX_CURIOUSITY = 1.0f;
    
    // Developmental parameters
    static constexpr float INITIAL_PLASTICITY = 1.0f;
    static constexpr float MATURE_PLASTICITY = 0.2f;
    static constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;
    static constexpr float MATURATION_PLASTICITY = 0.5f;
    
    // Novelty detection parameters
    static constexpr float MIN_NOVELTY = 0.0f;
    static constexpr float MAX_NOVELTY = 1.0f;
    
    // Prediction error bounds
    static constexpr float MAX_PREDICTION_ERROR = 1.0f;
    static constexpr float MIN_PREDICTION_ERROR = -1.0f;
    
    // Activity threshold for motor decoding
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    // Exploration parameters
    static constexpr float EXPLORATION_BASE_CHANCE = 0.3f;
    static constexpr float HIGH_CURIOUSITY_EXPLORATION = 0.5f;
};

} // namespace nlm