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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    
    // More sophisticated dopamine dynamics:
    // 1. Phasic bursts for unexpected rewards (prediction error)
    // 2. Tonic baseline for motivation
    // 3. Dopamine concentration affects both learning rate and stability
    
    // Phasic component (transient changes)
    float phasic = pImpl->level - pImpl->baseline;
    phasic = std::clamp(phasic, 0.0f, 1.0f);
    
    // Complex interaction between phasic and tonic components
    // Medium dopamine levels promote exploration and flexible learning
    // High dopamine levels can destabilize already-learned weights
    float plasticityFactor = 0.5f + 0.5f * pImpl->level + 0.2f * phasic;
    
    // Apply non-linear transformation for biological realism
    // Dopamine has an inverted-U effect on learning rate
    float optimalDopamine = 0.5f;
    float dopamineStrength = std::abs(pImpl->level - optimalDopamine);
    float nonLinearFactor = std::exp(-2.0f * dopamineStrength * dopamineStrength);
    
    plasticityFactor *= nonLinearFactor;
    
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    
    float deltaTime = static_cast<float>(dt);
    
    // Continuous decay toward baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // Additional factors: spontaneous activity, ambient noise
    // Not implemented yet but could add stochastic component
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    
    // More sophisticated reward prediction error (RPE) implementation:
    // According to dopamine literature, dopamine neurons respond to
    // unexpected rewards (positive RPE) and omitted expected rewards (negative RPE)
    
    // Burst intensity depends on reward magnitude and prediction error
    float burstMagnitude = reward * pImpl->releaseRate;
    
    // Add prediction error component (if implemented)
    // For now, just respond to actual reward delivery
    
    // Apply with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    
    // Implementing proper RPE signaling per dopamine literature:
    // - Positive error (unexpected reward): increase dopamine (learning signal)
    // - Negative error (expected but omitted reward): decrease dopamine
    
    // Error can be positive or negative
    float errorMagnitude = std::abs(error);
    bool isPositive = error > 0.0f;
    
    if (isPositive) {
        // Phasic increase for positive prediction error
        float increment = errorMagnitude * pImpl->releaseRate * 1.5f;  // Amplify RPE signal
        pImpl->level = std::min(pImpl->peak, pImpl->level + increment);
    } else {
        // Phasic decrease for negative prediction error
        float decrement = errorMagnitude * pImpl->decayRate;
        pImpl->level = std::max(0.0f, pImpl->level - decrement);
    }
}

} // namespace nlm
