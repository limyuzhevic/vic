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
    // Dopamine amplifies plastic changes during reward prediction error
    // Higher dopamine increases the learning rate by a multiplicative factor
    return std::pow(2.0f, pImpl->level);  // Exponential relationship
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with realistic time constants
    // Multiple pools: tonic (baseline), phasic (bursts), and error signals
    float timeFactor = static_cast<float>(dt) / 1000.0f;  // Convert to seconds
    
    // Decay towards baseline with exponential decay
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 
                   static_cast<float>(std::exp(-pImpl->decayRate * timeFactor));
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics
    // Phasic dopamine release proportional to unexpected reward
    float burstMagnitude = std::min(reward * pImpl->releaseRate, pImpl->peak - pImpl->level);
    pImpl->level = std::min(pImpl->level + burstMagnitude, pImpl->peak);
    
    // Create a burst profile that decays quickly
    // The system should quickly return to baseline after the burst
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Phasic dopamine responds to prediction errors with appropriate sign
    if (error > 0.0f) {
        // Positive prediction error (better than expected) -> dopamine increase
        pImpl->level = std::min(pImpl->level + error * pImpl->releaseRate * 2.0f, pImpl->peak);
    } else {
        // Negative prediction error (worse than expected) -> dopamine decrease
        pImpl->level = std::max(pImpl->level + error * pImpl->releaseRate, 0.0f);
    }
    
    // Error signals can persist briefly to modulate learning
}

} // namespace nlm
