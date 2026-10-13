// Development Parameters
// Synaptic development, maturation, and plasticity constants

#ifndef SRC_CORE_DEVELOPMENT_CONSTANTS_HPP
#define SRC_CORE_DEVELOPMENT_CONSTANTS_HPP

namespace DevelopmentConstants {
    // Synaptogenesis
    static constexpr float SYNAPTOGENESIS_FORMATION_RATE = 0.001f;           // Synaptogenesis formation rate
    static constexpr float SYNAPTOGENESIS_TARGET_DENSITY = 0.1f;             // Target synaptic density
    
    // Maturation
    static constexpr float MATURATION_TIME_CONSTANT = 20.0f;                 // Maturation time constant (ms)
    static constexpr float MATURATION_THRESHOLD = -55.0f;                    // Maturation threshold (mV)
    static constexpr float MATURATION_RESTING_POTENTIAL = -70.0f;            // Mature resting potential (mV)
    static constexpr float MATURATION_RESET_POTENTIAL = -70.0f;              // Mature reset potential (mV)
    
    // Pruning
    static constexpr float PRUNING_TIME_CONSTANT = 20.0f;                    // Pruning time constant (ms)
    static constexpr float PRUNING_THRESHOLD = -55.0f;                       // Pruning threshold (mV)
    
    // General development
    static constexpr float DEVELOPMENT_DECAY_FACTOR = 0.99f;                 // Development decay factor
    static constexpr float DEVELOPMENT_MIN_FACTOR = 0.05f;                   // Development minimum factor
    static constexpr float DEVELOPMENT_UPDATE_FACTOR = 0.95f;                // Development update factor
    
    // Planning and learning
    static constexpr size_t PLANNING_DEPTH = 5;                             // Planning depth
    static constexpr float PLANNING_CONFIDENCE_BASE = 0.5f;                 // Planning confidence base
    static constexpr float PLANNING_CAPABILITY_BASE = 0.5f;                 // Planning capability base
    static constexpr float PLANNING_CONSISTENCY = 0.5f;                     // Planning consistency
    
    // Developmental aging
    static constexpr float DEVELOPMENT_MAX_AGE = 60.0f;                      // Maximum developmental age (steps)
    static constexpr float PLASTICITY_MODIFIER_MAX = 0.8f;                   // Maximum plasticity modifier
    
    // Action selection
    static constexpr float ACTION_SELECTION_THRESHOLD = 0.5f;               // Action selection threshold
    static constexpr float ACTION_QUALITY_WEIGHT = 0.5f;                    // Action quality weight
}

#endif // SRC_CORE_DEVELOPMENT_CONSTANTS_HPP