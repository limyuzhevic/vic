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
    // IMPROVEMENT: Implement realistic dopamine-modulated plasticity factor
    // Real dopamine effects: high dopamine enhances LTP, low dopamine impairs plasticity
    // Also includes temporal dynamics and reward prediction error integration
    
    if (pImpl->level <= 0.0f) {
        // No dopamine -> severely impaired plasticity
        return 0.1f;
    }
    
    // Nonlinear relationship: plasticity increases with dopamine but saturates
    float baseFactor = 0.2f;  // Baseline plasticity
    float dynamicFactor = 1.8f * pImpl->level;  // Dopamine-dependent enhancement
    float saturation = 1.0f / (1.0f + std::exp(-5.0f * (pImpl->level - 0.5f)));  // Sigmoidal saturation
    
    // Realistic plasticity dynamics: combines baseline, dopamine boost, and saturation
    float plasticityFactor = baseFactor + dynamicFactor * saturation;
    
    // Clamp to reasonable bounds
    return std::clamp(plasticityFactor, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // IMPROVEMENT: Implement real dopamine dynamics with proper decay and baseline
    // Real dopamine: surges on reward, decays with characteristic time constant
    // Includes baseline activity and burst dynamics
    
    // More realistic decay with baseline
    float decayRate = std::max(0.05f, pImpl->decayRate * (1.0f - pImpl->level));  // Slower decay when high
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    // For now, we simulate the temporal dynamics of dopamine release
    float burstAmount = reward * pImpl->releaseRate * static_cast<float>(1.0 + 0.5f * std::abs(reward));
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    
    // Add some noise to simulate biological variability
    pImpl->level *= 1.0f + (std::rand() % 100 / 100.0f - 0.5f) * 0.1f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // IMPROVEMENT: Implement realistic reward prediction error signaling
    // Real RPE signaling: negative prediction error (surprise) increases dopamine more than positive RPE
    // This implements the temporal difference learning prediction error
    
    float rpeMagnitude = std::abs(error);
    
    // More realistic RPE dynamics: surprise (negative RPE) triggers stronger dopamine response
    // than expected reward (positive RPE)
    float rpeEffect = 0.0f;
    if (error < 0.0f) {
        // Negative RPE (surprise/prediction error): strong dopamine burst
        rpeEffect = rpeMagnitude * 2.0f * pImpl->releaseRate;
    } else if (error > 0.0f) {
        // Positive RPE (expected reward): moderate dopamine response
        rpeEffect = rpeMagnitude * 0.8f * pImpl->releaseRate;
    }
    
    pImpl->level = std::max(0.0f, pImpl->level + rpeEffect);
}

} // namespace nlm
