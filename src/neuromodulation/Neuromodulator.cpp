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
    // Enhanced dopamine-modulated plasticity factor
    // Based on more realistic dopamine dynamics
    float saturation = pImpl->level * pImpl->level;  // Non-linear effect
    return 0.1f + 0.8f * saturation;  // Range from 0.1 to 0.9
}

void Dopamine::update(TimestepDuration dt) {
    // More realistic dopamine dynamics with exponential decay
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline with exponential function
    float decayFactor = std::exp(-pImpl->decayRate * dtFloat);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling with temporal difference learning
    // Dopamine burst for unexpected reward
    float rewardIntensity = std::min(reward, 2.0f);  // Cap maximum reward response
    
    // Add reward with saturation
    pImpl->level = std::min(1.0f, pImpl->level + rewardIntensity * pImpl->releaseRate * 0.5f);
    
    // Encode reward prediction error if we have a prediction
    // This will be implemented when prediction system is integrated
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    // Positive error (better than expected) -> dopamine increase
    // Negative error (worse than expected) -> dopamine decrease
    
    // More nuanced error response
    float errorResponse = error * pImpl->releaseRate * 0.3f;
    
    // Complex error integration
    if (error > 0.0f) {
        // Phasic increase for positive prediction errors
        pImpl->level = std::min(1.0f, pImpl->level + errorResponse);
    } else {
        // Phasic decrease for negative prediction errors
        pImpl->level = std::max(0.0f, pImpl->level + errorResponse);
    }
}

} // namespace nlm
