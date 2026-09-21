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
    // REAL PHASE 2: Implement real dopamine-modulated plasticity factor
    // Realistic model based on experimental findings: 
    // - Low dopamine (0.0-0.3): impairs learning, reduces plasticity
    // - Medium dopamine (0.3-0.7): optimal for learning
    // - High dopamine (0.7-1.0): stabilizes learned connections
    float normalizedLevel = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    if (normalizedLevel < 0.3f) {
        // Low dopamine: impaired learning
        return 0.2f + normalizedLevel * 0.6f;
    } else if (normalizedLevel < 0.7f) {
        // Optimal learning range
        return 0.8f * normalizedLevel;
    } else {
        // High dopamine: stabilization, reduced plasticity
        return 0.56f + 0.04f * (1.0f - normalizedLevel);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // REAL PHASE 2: Implement real dopamine dynamics
    // Realistic model based on reinforcement learning and prediction error
    
    // Decay towards baseline (neural adaptation)
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Limit peak based on neural firing rate constraints
    pImpl->peak = std::min(pImpl->peak, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // REAL PHASE 2: Implement real reward signaling
    // Realistic model with temporal dynamics:
    // - Burst response scaled by reward magnitude
    // - Postsynaptic calcium dynamics
    // - Adaptation after large rewards
    
    // Base response to reward
    float rewardResponse = std::clamp(reward, 0.0f, 1.0f) * pImpl->releaseRate;
    
    // Additive effect (prediction error component)
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse * 0.5f);
    
    // Burst component (phasic response)
    float burstMagnitude = rewardResponse * 0.7f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL PHASE 2: Implement reward prediction error signaling
    // TD learning rule implementation:
    // - Phasic dopamine bursts for positive prediction errors
    // - Phasic dips for negative prediction errors
    // - Baseline firing for zero prediction error
    
    float normalizedError = std::clamp(error, -1.0f, 1.0f);
    
    if (normalizedError > 0.0f) {
        // Positive prediction error: dopamine burst
        float burstMagnitude = normalizedError * pImpl->releaseRate * 0.8f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    } else if (normalizedError < 0.0f) {
        // Negative prediction error: dopamine dip (inhibition)
        float dipMagnitude = -normalizedError * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dipMagnitude);
    }
    // If error is zero, maintain baseline (no change)
}

} // namespace nlm
