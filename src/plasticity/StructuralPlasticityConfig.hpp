#pragma once

namespace nlm {

// Structural Plasticity configuration constants
struct StructuralPlasticityConfig {
    // Weight change thresholds
    static constexpr float WEIGHT_CHANGE_THRESHOLD_POSITIVE = 0.01f;
    static constexpr float WEIGHT_CHANGE_THRESHOLD_NEGATIVE = 0.001f;
    
    // Synaptogenesis and pruning parameters
    static constexpr float SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float PRUNING_RATE = 0.00001f;
    
    // Development stage plasticity modifiers
    static constexpr float PLASTICITY_INITIAL = 1.0f;
    static constexpr float PLASTICITY_CRITICAL_PERIOD = 0.8f;
    static constexpr float PLASTICITY_MATURATION = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Development age thresholds
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
};

} // namespace nlm
