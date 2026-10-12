// NLM Configuration Constants
// Extracted magic numbers for better maintainability and readability

#pragma once

namespace nlm {

// Neuron distribution constants
namespace AgentBrainConstants {
    // Number of motor neuron action groups
    constexpr size_t MOTOR_ACTION_GROUPS = 6;
    
    // Motor action group indices
    constexpr size_t MOTOR_FORWARD_GROUP = 0;
    constexpr size_t MOTOR_BACKWARD_GROUP = 1;
    constexpr size_t MOTOR_TURN_LEFT_GROUP = 2;
    constexpr size_t MOTOR_TURN_RIGHT_GROUP = 3;
    constexpr size_t MOTOR_INTERACT_GROUP = 4;
    constexpr size_t MOTOR_WAIT_GROUP = 5;
    
    // Number of sensory neuron groups
    constexpr size_t SENSORY_GROUPS = 4;
    
    // Sensory group indices
    constexpr size_t SENSORY_VISION_GROUP = 0;
    constexpr size_t SENSORY_TOUCH_GROUP = 1;
    constexpr size_t SENSORY_INTERNAL_GROUP = 2;
    constexpr size_t SENSORY_PROPRIOCEPTION_GROUP = 3;
    
    // Expected sensory input sizes (based on agent configuration)
    constexpr size_t SIZEOF_VISION_INPUT = 256;  // 16x16 grid
    constexpr size_t SIZEOF_TOUCH_INPUT = 8;
    constexpr size_t SIZEOF_INTERNAL_INPUT = 4;
    constexpr size_t SIZEOF_PROPRIOCEPTIVE_INPUT = 6;
    constexpr size_t EXPECTED_SENSORY_INPUT_SIZE = 
        SIZEOF_VISION_INPUT + SIZEOF_TOUCH_INPUT + 
        SIZEOF_INTERNAL_INPUT + SIZEOF_PROPRIOCEPTIVE_INPUT;
    
    // Expected motor output size
    constexpr size_t EXPECTED_MOTOR_OUTPUT_SIZE = 6;
    
    // Neuromodulation constants
    constexpr float MAX_NEUROMODULATION_LEVEL = 1.0f;
    constexpr float MIN_NEUROMODULATION_LEVEL = -1.0f;
    
    // Curiosity and exploration constants
    constexpr float CURIOSITY_THRESHOLD = 0.3f;
    constexpr float MAX_CURIOSITY_LEVEL = 1.0f;
    constexpr float MIN_CURIOSITY_LEVEL = 0.0f;
    constexpr float EXPLORATION_MAX_CHANCE = 0.3f;  // Maximum random exploration chance
    constexpr size_t MAX_RANDOM_EXPLORATION_CHOICES = 8;
    
    // Plasticity constants
    constexpr float MAX_PLASTICITY_FACTOR = 2.0f;
    constexpr float MIN_PLASTICITY_FACTOR = 0.1f;
    constexpr float DEFAULT_PLASTICITY_MODIFIER = 1.0f;
    
    // Development stages (in simulation time steps)
    constexpr double DEVELOPMENT_INITIAL_DURATION = 60.0;     // ~1 minute
    constexpr double DEVELOPMENT_CRITICAL_PERIOD_DURATION = 240.0;  // ~4 minutes
    constexpr double DEVELOPMENT_MATURATION_DURATION = 540.0;   // ~9 minutes
    
    // Novelty detection
    constexpr float NOVELTY_DECAY_RATE = 0.99f;
    
    // Reward modulation
    constexpr float REWARD_EXPECTATION_DECAY = 0.95f;
    constexpr float REWARD_EXPECTATION_UPDATE = 0.05f;
    
    // Weight change thresholds
    constexpr float SIGNIFICANT_WEIGHT_CHANGE = 0.01f;
    
    // Structural plasticity rates (base rates)
    constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    constexpr float PRUNING_BASE_RATE = 0.00001f;
    
    // Current injection scaling factors
    constexpr float VISION_CURRENT_SCALE = 5.0f;
    constexpr float TOUCH_CURRENT_SCALE = 8.0f;
    constexpr float INTERNAL_CURRENT_SCALE = 5.0f;
    constexpr float PROPRIOCEPTIVE_CURRENT_SCALE = 3.0f;
    
    // STDP parameters
    constexpr float STDP_LTP_WEIGHT_BASE = 0.01f;
    constexpr float STDP_LTD_WEIGHT_BASE = 0.012f;
    constexpr float STDP_TAU_BASE = 20.0f;
    
    // Eligibility trace
    constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;
}

} // namespace nlm
