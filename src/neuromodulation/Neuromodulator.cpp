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
    // Phase 2: Real dopamine-modulated plasticity factor
    // Implements biologically plausible dopamine effects on plasticity
    // Higher dopamine levels increase both LTP and LTD plasticity
    // With saturation at high levels to prevent runaway excitation
    
    // Normalized dopamine level (0-1)
    float normLevel = std::min(1.0f, pImpl->level / pImpl->peak);
    
    // Base plasticity factor
    float baseFactor = 0.1f;
    
    // Dopamine enhances plasticity non-linearly
    // Inverted U-shaped curve (optimal around 0.7-0.8)
    float optimalLevel = 0.75f;
    float optimalWidth = 0.3f;
    
    float enhancement = std::exp(-std::pow((normLevel - optimalLevel) / optimalWidth, 2.0f)) * 2.0f;
    
    return baseFactor * (1.0f + enhancement);
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 2: Real dopamine dynamics
    // Implements biologically realistic dopamine dynamics with decay and release
    float timeStep = static_cast<float>(dt);
    
    // Decay towards baseline (exponential decay)
    if (pImpl->level > pImpl->baseline) {
        float decayAmount = pImpl->decayRate * timeStep;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    }
    
    // Add small random fluctuations
    pImpl->level += (rng ? rng->getFloat() - 0.5f : 0.0f) * 0.01f * timeStep;
}

void Dopamine::signalReward(float reward) {
    // Phase 2: Real reward signaling
    // Implements reward prediction error-like signaling
    // Burst of dopamine on unexpected reward, attenuation on predicted reward
    
    // Scale reward to dopamine level
    float rewardEffect = reward * pImpl->releaseRate;
    
    // Clamp to peak level
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardEffect);
    
    // Small increase to baseline to account for tonic firing
    if (reward > 0.0f) {
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + reward * 0.1f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 2: Reward prediction error signaling
    // Implements dopamine response to RPE (dopamine prediction error)
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    
    // Scale RPE to dopamine change
    float errorEffect = error * pImpl->releaseRate * 2.0f;
    
    // Update level with error effect
    pImpl->level = std::clamp(pImpl->level + errorEffect, pImpl->baseline, pImpl->peak);
    
    // Update baseline based on long-term RPE
    if (error > 0.0f) {
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + error * 0.05f);
    }
}

} // namespace nlm
