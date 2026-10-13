// Plasticity Parameters
// STDP and synaptic plasticity constants

#ifndef SRC_CORE_PLASTICITY_CONSTANTS_HPP
#define SRC_CORE_PLASTICITY_CONSTANTS_HPP

#include "Types/Types.hpp"

namespace nlm {

namespace PlasticityConstants {
    // STDP time constants
    static constexpr float LTP_TIME_CONSTANT = 20.0f;         // Long-term potentiation time constant (ms)
    static constexpr float LTD_TIME_CONSTANT = 20.0f;         // Long-term depression time constant (ms)
    
    // STDP weights
    static constexpr float LTP_WEIGHT = 0.02f;                // LTP weight
    static constexpr float LTD_WEIGHT = 0.015f;               // LTD weight
    static constexpr float STP_WEIGHT = 0.001f;               // Short-term plasticity weight
    
    // Default delay
    static constexpr Delay DEFAULT_DELAY = 1;                  // Default synaptic delay (steps)
    
    // Short-term plasticity
    static constexpr float STP_FACILITATION_TAU = 100.0f;     // Short-term facilitation time constant (ms)
    static constexpr float STP_DEPRESSION_TAU = 100.0f;       // Short-term depression time constant (ms)
    static constexpr float STP_MAX_FACILITATION = 2.0f;       // Maximum facilitation factor
    static constexpr float STP_MAX_DEPRESSION = 2.0f;          // Maximum depression factor
    
    // Pruning and synaptogenesis
    static constexpr float PRUNING_RATE = 0.00001f;            // Synaptic pruning rate
    static constexpr float SYNAPTOGENESIS_RATE = 0.0001f;     // Synaptogenesis rate
    
    // Plasticity modulation
    static constexpr float PLASTICITY_MODIFIER_BASE = 0.5f;    // Base plasticity modifier
    static constexpr float PLASTICITY_CLAMP_MIN = 0.1f;        // Minimum plasticity clamp
    static constexpr float PLASTICITY_CLAMP_MAX = 2.0f;        // Maximum plasticity clamp
    
    // Additional plasticity constants
    static constexpr float MIN_WEIGHT_THRESHOLD = 0.05f;       // Minimum weight threshold for plasticity
    static constexpr float LTP_EXTRA_WEIGHT = 0.01f;          // Additional LTP weight component
    static constexpr float LTD_EXTRA_WEIGHT = 0.012f;         // Additional LTD weight component
    
    // Synaptic efficacy and utilization
    static constexpr float SYNAPTIC_EFFICIENCY_MIN = 0.0f;     // Minimum synaptic efficacy
    static constexpr float SYNAPTIC_EFFICIENCY_MAX = 1.0f;     // Maximum synaptic efficacy
    
    // Depression and utilization bounds
    static constexpr float DEPRESSION_U_MAX = 1.0f;            // Maximum depression utilization
    
    // Test constants
    static constexpr size_t STDP_TEST_MIN_SYNAPSES = 5;      // Minimum synapses for STDP test
    static constexpr int STDP_TEST_TRIALS = 50;               // Number of STDP test trials
    static constexpr float STDP_INPUT_CURRENT = 60.0f;       // Input current for STDP test (nA)
    static constexpr float STDP_LEARNING_THRESHOLD = 0.001f; // Minimum learning threshold for STDP detection
}

} // namespace nlm

#endif // SRC_CORE_PLASTICITY_CONSTANTS_HPP