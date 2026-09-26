// ============================================================================
// VALUE SCALING UTILITIES EXAMPLE
// ============================================================================

#include <iostream>
#include "Utilities.hpp"

int main() {
    std::cout << "=== Value Scaling Utilities Example ===\n";
    
    // Example 1: Scale vision intensity from [0,1] to [-1,1]
    float visionIntensity = 0.75f;
    float normalized = nlm::scaleToRange(visionIntensity, 0.0f, 1.0f, -1.0f, 1.0f);
    std::cout << "Vision intensity " << visionIntensity << " -> " << normalized << "\n";
    
    // Example 2: Normalize to signed range (common pattern: (value * 2.0f - 1.0f))
    float signedValue = nlm::normalizeToSigned(visionIntensity);
    std::cout << "Vision intensity normalized to signed: " << signedValue << "\n";
    
    // Example 3: Apply offset-scale transformation (common pattern: (value * 2.0f - 1.0f) * 5.0f)
    float motorScaled = nlm::applyOffsetScale(visionIntensity, -1.0f, 5.0f);
    std::cout << "Vision intensity offset-scaled: " << motorScaled << "\n";
    
    // Example 4: Scale motor command from [0,1] to [0,255]
    float motorCmd = 0.8f;
    uint8_t pwm = static_cast<uint8_t>(nlm::scaleToRange(motorCmd, 0.0f, 1.0f, 0.0f, 255.0f));
    std::cout << "Motor command " << motorCmd << " -> PWM: " << pwm << "\n";
    
    return 0;
}
