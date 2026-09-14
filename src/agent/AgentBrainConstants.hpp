#pragma once

namespace nlm {

// AgentBrain configuration constants
struct AgentBrainConfig {
    // Default initialization values
    static constexpr float INITIAL_DOPAMINE_LEVEL = 0.0f;
    static constexpr float INITIAL_NOVELTY_LEVEL = 0.0f;
    static constexpr float INITIAL_CURIOUSY_LEVEL = 0.0f;
    static constexpr float INITIAL_PREDICTION_ERROR = 0.0f;
    static constexpr float INITIAL_EXPECTED_REWARD = 0.0f;
    static constexpr double INITIAL_DEVELOPMENTAL_AGE = 0.0;
    static constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
    static constexpr float INITIAL_SENSORY_NOVELTY_DECAY = 0.99f;
    
    // Plasticity modifier values for developmental stages
    static constexpr float PLASTICITY_INITIAL = 1.0f;
    static constexpr float PLASTICITY_CRITICAL_PERIOD = 0.8f;
    static constexpr float PLASTICITY_MATURATION = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Developmental stage age thresholds
    static constexpr double INITIAL_STAGE_AGE = 60.0;  // ~1 minute
    static constexpr double CRITICAL_PERIOD_AGE = 300.0;  // ~5 minutes
    static constexpr double MATURATION_AGE = 900.0;  // ~15 minutes
    
    // Neuromodulation value ranges
    static constexpr float NEUROMODULATION_MIN = 0.0f;
    static constexpr float NEUROMODULATION_MAX = 1.0f;
    static constexpr float PLASTICITY_MODIFIER_MIN = 0.1f;
    static constexpr float PLASTICITY_MODIFIER_MAX = 2.0f;
    
    // Reward and prediction error weights
    static constexpr float REWARD_WEIGHT = 0.05f;
    static constexpr float PREDICTION_ERROR_CLAMP_MIN = -1.0f;
    static constexpr float PREDICTION_ERROR_CLAMP_MAX = 1.0f;
    
    // Synaptogenesis and pruning factors
    static constexpr float SYNAPTOGENESIS_FACTOR = 0.0001f;
    static constexpr float PRUNING_FACTOR = 0.00001f;
    
    // System control flags
    static constexpr bool DEFAULT_REWARD_MODULATION_ENABLED = true;
    static constexpr bool DEFAULT_STRUCTURAL_PLASTICITY_ENABLED = true;
    static constexpr bool DEFAULT_DEVELOPMENT_ENABLED = true;
    static constexpr bool DEFAULT_CURIOUSY_ENABLED = true;
    
    // Sensory processing constants
    static constexpr size_t DEFAULT_SENSORY_INPUT_SIZE = 284;  // Vision (256) + touch (8) + internal (4) + proprioception (6)
    static constexpr size_t DEFAULT_MOTOR_OUTPUT_SIZE = 6;
    
    // Sensory weight and scaling factors
    static constexpr float SENSORY_VISION_SCALE = 5.0f;
    static constexpr float SENSORY_TOUCH_SCALE = 8.0f;
    static constexpr float SENSORY_INTERNAL_SCALE = 5.0f;
    static constexpr float SENSORY_PROPRIOCEPTION_SCALE = 3.0f;
    static constexpr float SENSORY_INTERNAL_CENTER = -1.0f;
    static constexpr float SENSORY_PROPOTOPECTION_CENTER = -1.0f;
};

} // namespace nlm
