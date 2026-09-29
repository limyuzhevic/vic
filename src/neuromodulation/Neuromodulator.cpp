#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Based on dopamine level: higher DA = higher plasticity up to a point
    // Implements dopamine-dependent learning modulation
    float plasticity = 0.3f + 0.7f * pImpl->level;
    
    // Apply biological constraints: very high DA can inhibit plasticity ( Inverted U-shape)
    if (pImpl->level > 0.8f) {
        plasticity = 0.5f + 0.4f * (1.0f - pImpl->level);
    }
    
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple time constants
    // Phasic responses to rewards, tonic background activity
    
    float dtf = static_cast<float>(dt);
    
    // Phasic decay: rapid return to baseline after reward prediction
    const float phasicDecayRate = 0.2f; // Fast decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - phasicDecayRate * dtf);
    
    // Tonic adaptation: slow drift towards baseline over longer timescales
    const float tonicAdaptationRate = 0.05f; // Slow adaptation
    pImpl->level += (pImpl->baseline - pImpl->level) * tonicAdaptationRate * dtf * 10.0f;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal dynamics
    // Implements phasic burst response to unexpected rewards
    
    // Phasic response strength depends on reward magnitude and prediction error
    float burstMagnitude = reward * pImpl->releaseRate;
    
    // Normalize burst magnitude to reasonable bounds
    burstMagnitude = std::min(burstMagnitude, 0.5f);
    
    // Add phasic response to current level
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Reset prediction-related dopamine for next trial
    // This would be set by reward prediction error signaling
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Phasic DA response to unexpected prediction errors (both positive and negative)
    
    if (std::abs(error) > 0.01f) { // Only respond to meaningful prediction errors
        // Sign of error determines direction of response
        float errorResponse = error > 0.0f ? 0.3f : -0.2f;
        
        // Apply response with saturation
        pImpl->level = std::clamp(pImpl->level + errorResponse * pImpl->releaseRate, 
                                  pImpl->baseline, pImpl->peak);
    }
}

} // namespace nlm
