// Core Constants Implementation
// 
// This file provides implementations for the constants defined in Constants.hpp
// and includes utility functions used throughout the NLM codebase.

#include "Constants.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// =============================================
// CONSTANTS NAMESPACE IMPLEMENTATION
// =============================================

// Note: Most constants are defined as constexpr inline in the header file
// to avoid linker duplication issues. This file can be used for:
// 1. Runtime constants (if needed)
// 2. Configuration initialization
// 3. Utility function implementations
// 4. Constants validation and checks

// =============================================
// RUNTIME CONSTANTS (if needed)
// =============================================

// Runtime versions of constants that may need to be modified at runtime
namespace RuntimeConstants {
    // Simulation timestep can be modified at runtime
    inline double simulationTimestep = Constants::SIMULATION_TIMESTEP_DEFAULT;
    
    // Performance monitoring
    inline float cpuUsageThreshold = Constants::PERFORMANCE_UTILIZATION_THRESHOLD;
    inline double memoryLimit = 0.0;  // Unlimited by default
    
    // Environment settings
    inline int environmentWidth = Constants::ENVIRONMENT_DEFAULT_WIDTH;
    inline int environmentHeight = Constants::ENVIRONMENT_DEFAULT_HEIGHT;
    
    // Constants that can be scaled during development
    inline float plasticityScaling = 1.0f;
    inline float curiosityScaling = 1.0f;
}

// =============================================
// CONFIGURATION INTEGRATION
// =============================================

// Initialize configuration with default constants
void initializeDefaultsFromConstants(Config& config) {
    // Core simulation
    config.set("simulation_timestep", Constants::Defaults::simulation_timestep);
    config.set("random_seed", Constants::Defaults::random_seed);
    
    // Brain architecture
    config.set("neuron_count", Constants::Defaults::neuron_count);
    config.set("region_count", Constants::Defaults::region_count);
    config.set("connection_probability", Constants::Defaults::connection_probability);
    
    // Plasticity systems
    config.set("stdp_ltp_weight", Constants::Defaults::stdp_ltp_weight);
    config.set("stdp_ltd_weight", Constants::Defaults::stdp_ltd_weight);
    config.set("stdp_time_constant", Constants::SYNAPTIC_RECOVERY_TIME_CONST);
    config.set("hebbian_learning_rate", Constants::Defaults::hebbian_learning_rate);
    
    // Development
    config.set("synaptogenesis_rate", Constants::Defaults::synaptogenesis_rate);
    config.set("pruning_rate", Constants::Defaults::pruning_rate);
    
    // Neuromodulation
    config.set("dopamine_baseline", Constants::Defaults::dopamine_baseline);
    config.set("reward_discount_factor", Constants::Defaults::reward_discount_factor);
    
    // Environment
    config.set("environment_name", Constants::Defaults::environment_name);
    config.set("environment_width", Constants::Defaults::environment_width);
    config.set("environment_height", Constants::Defaults::environment_height);
    
    // Logging
    config.set("log_level", Constants::Defaults::log_level);
    config.set("log_to_file", Constants::Defaults::log_to_file);
    config.set("log_filename", Constants::Defaults::log_filename);
    
    // Performance
    config.set("visualization_enabled", Constants::Defaults::visualization_enabled);
    config.set("visualization_update_rate", Constants::Defaults::visualization_update_rate);
    
    // Simulation limits
    config.set("max_simulation_steps", Constants::Defaults::max_simulation_steps);
    config.set("simulation_time_limit", Constants::Defaults::simulation_time_limit);
}

// Validate constants against system constraints
bool validateConstants() {
    // Check that basic constraints are satisfied
    if (Constants::SIMULATION_TIMESTEP_MIN <= 0.0) {
        return false;  // Invalid minimum timestep
    }
    
    if (Constants::SIMULATION_TIMESTEP_MAX <= Constants::SIMULATION_TIMESTEP_MIN) {
        return false;  // Invalid timestep range
    }
    
    if (Constants::DEVELOPMENT_INITIAL_DURATION < 0.0) {
        return false;  // Invalid developmental duration
    }
    
    if (Constants::NEURAL_WEIGHT_MIN >= Constants::NEURAL_WEIGHT_MAX) {
        return false;  // Invalid weight bounds
    }
    
    if (Constants::MEMBRANE_POTENTIAL_THRESHOLD < 0.0 || 
        Constants::MEMBRANE_POTENTIAL_THRESHOLD > Constants::MEMBRANE_POTENTIAL_MAX) {
        return false;  // Invalid firing threshold
    }
    
    return true;
}

// =============================================
// UTILITY FUNCTIONS
// =============================================

// Mathematical utility functions
// These are inline implementations matching those in Constants.hpp

inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline float exponentialDecay(float value, float rate, double dt) {
    return value * std::pow(rate, dt);
}

inline float normalize(float value, float min, float max) {
    if (max <= min) return 0.0f;
    float normalized = (value - min) / (max - min);
    return clamp(normalized, 0.0f, 1.0f);
}

inline float denormalize(float value, float min, float max) {
    return min + value * (max - min);
}

// Array utility functions
inline size_t safeArrayAccess(const std::vector<float>& arr, size_t index) {
    return (index < arr.size()) ? arr[index] : 0.0f;
}

inline size_t clampIndex(size_t index, size_t min, size_t max) {
    return std::max(min, std::min(index, max - 1));
}

// =============================================
// CONSTANTS VALIDATION AND TESTING
// =============================================

// Test that constants are consistent with each other
bool testConstantsConsistency() {
    // Test weight bounds consistency
    if (Constants::HEBBIAN_WEIGHT_MIN > Constants::HEBBIAN_WEIGHT_MAX) {
        return false;
    }
    
    if (Constants::STDP_WEIGHT_MIN > Constants::STDP_WEIGHT_MAX) {
        return false;
    }
    
    // Test development stage durations
    if (Constants::DEVELOPMENT_INITIAL_DURATION <= 0.0) {
        return false;
    }
    
    if (Constants::DEVELOPMENT_CRITICAL_PERIOD_DURATION <= Constants::DEVELOPMENT_INITIAL_DURATION) {
        return false;
    }
    
    if (Constants::DEVELOPMENT_MATURATION_DURATION <= Constants::DEVELOPMENT_CRITICAL_PERIOD_DURATION) {
        return false;
    }
    
    if (Constants::DEVELOPMENT_ADULT_DURATION <= Constants::DEVELOPMENT_MATURATION_DURATION) {
        return false;
    }
    
    // Test sensory array sizes
    if (Constants::SENSORY_VISION_DIMS == 0) {
        return false;
    }
    
    if (Constants::SENSORY_TOUCH_SIZE == 0) {
        return false;
    }
    
    if (Constants::SENSORY_INTERNAL_SIZE == 0) {
        return false;
    }
    
    if (Constants::SENSORY_PROPRIOCEPTION_SIZE == 0) {
        return false;
    }
    
    // Test motor action size
    if (Constants::MOTOR_ACTION_SIZE == 0) {
        return false;
    }
    
    return true;
}

// Print constants to log (for debugging)
void logConstants() {
    NLM_LOG_INFO("=== NLM Constants ===");
    NLM_LOG_INFO("Physics/Neural:");
    NLM_LOG_INFO("  Neural weight bounds: [" + std::to_string(Constants::NEURAL_WEIGHT_MIN) + ", " + std::to_string(Constants::NEURAL_WEIGHT_MAX) + "]");
    NLM_LOG_INFO("  Firing threshold: " + std::to_string(Constants::MEMBRANE_POTENTIAL_THRESHOLD));
    NLM_LOG_INFO("  Current injection scales: Vision=" + std::to_string(Constants::CURRENT_INJECTION_SCALE_VISION) + 
                 ", Touch=" + std::to_string(Constants::CURRENT_INJECTION_SCALE_TOUCH) + 
                 ", Internal=" + std::to_string(Constants::CURRENT_INJECTION_SCALE_INTERNAL) + 
                 ", Proprioception=" + std::to_string(Constants::CURRENT_INJECTION_SCALE_PROPRIOCEPTION));
    
    NLM_LOG_INFO("Time:");
    NLM_LOG_INFO("  Default simulation timestep: " + std::to_string(Constants::SIMULATION_TIMESTEP_DEFAULT) + "s");
    NLM_LOG_INFO("  Development stages: Initial=" + std::to_string(Constants::DEVELOPMENT_INITIAL_DURATION) + 
                 "s, Critical=" + std::to_string(Constants::DEVELOPMENT_CRITICAL_PERIOD_DURATION) + 
                 "s, Mature=" + std::to_string(Constants::DEVELOPMENT_MATURATION_DURATION) + 
                 "s, Adult=" + std::to_string(Constants::DEVELOPMENT_ADULT_DURATION) + "s");
    
    NLM_LOG_INFO("Plasticity:");
    NLM_LOG_INFO("  STDP: LTP weight=" + std::to_string(Constants::STDP_LTP_WEIGHT_DEFAULT) + 
                 ", LTD weight=" + std::to_string(Constants::STDP_LTD_WEIGHT_DEFAULT) + 
                 ", time constant=" + std::to_string(Constants::SYNAPTIC_RECOVERY_TIME_CONST));
    NLM_LOG_INFO("  Hebbian: Learning rate=" + std::to_string(Constants::HEBBIAN_LEARNING_RATE_DEFAULT));
    
    NLM_LOG_INFO("Sensory:");
    NLM_LOG_INFO("  Vision: " + std::to_string(Constants::SENSORY_VISION_SIZE) + "x" + std::to_string(Constants::SENSORY_VISION_SIZE) + 
                 " (" + std::to_string(Constants::SENSORY_VISION_DIMS) + " elements)");
    NLM_LOG_INFO("  Touch: " + std::to_string(Constants::SENSORY_TOUCH_SIZE) + " sensors");
    NLM_LOG_INFO("  Internal: " + std::to_string(Constants::SENSORY_INTERNAL_SIZE) + " sensors");
    NLM_LOG_INFO("  Proprioception: " + std::to_string(Constants::SENSORY_PROPRIOCEPTION_SIZE) + " sensors");
    
    NLM_LOG_INFO("Motor:");
    NLM_LOG_INFO("  Actions: " + std::to_string(Constants::MOTOR_ACTION_SIZE) + " possible actions");
    NLM_LOG_INFO("  Environment: " + std::to_string(Constants::ENVIRONMENT_DEFAULT_WIDTH) + 
                 "x" + std::to_string(Constants::ENVIRONMENT_DEFAULT_HEIGHT) + " grid");
    
    NLM_LOG_INFO("=== Constants End ===");
}

// =============================================
// CONSTANTS.CPP IMPLEMENTATION COMPLETE
// =============================================
