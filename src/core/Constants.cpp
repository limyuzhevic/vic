#include "core/Constants.hpp"

namespace nlm {

// Constants implementation - providing concrete definitions for compile-time constants

// Additional runtime constants that might need adjustment
namespace runtime {
    // Default values that can be overridden at runtime
    namespace defaults {
        constexpr float weight_strength_threshold = 0.01f;
        constexpr float weight_weakness_threshold = -0.01f;
        constexpr float weight_change_threshold = 0.001f;
        constexpr float neuron_injection_current = 50.0f;
        constexpr float moderate_injection_current = 30.0f;
        constexpr float weak_injection_current = 5.0f;
    }
}

// Utility functions for constants
namespace utils {
    
    // Clamp function for float values
    inline float clamp(float value, float min, float max) {
        return value < min ? min : (value > max ? max : value);
    }
    
    // Saturate function for float values
    inline float saturate(float value, float maxAbs) {
        return value > maxAbs ? maxAbs : (value < -maxAbs ? -maxAbs : value);
    }
    
    // Normalize value to range [0, 1]
    inline float normalize(float value, float min, float max) {
        if (max == min) return 0.5f;
        return (value - min) / (max - min);
    }
    
    // Linear interpolation
    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
} // namespace utils

} // namespace nlm