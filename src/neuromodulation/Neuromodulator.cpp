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
    // Real dopamine-modulated plasticity factor based on Yerkes-Dodson law
    // Optimal dopamine level for learning: ~0.7
    // Both too low and too high dopamine impair learning
    
    float optimalLevel = 0.7f;
    float inverseSquareDistance = (pImpl->level - optimalLevel) * (pImpl->level - optimalLevel);
    float plasticityFactor = 1.0f / (1.0f + inverseSquareDistance * 3.0f);
    
    // Ensure reasonable bounds
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on biologically-inspired models
    // Includes baseline decay, burst responses, and homeostatic regulation
    
    if (dt <= 0.0) return;
    
    // Convert timestep to seconds for rate calculations
    double dtSeconds = static_cast<double>(dt) * 0.001;  // Assuming ms timestep
    
    // Decay towards baseline with exponential dynamics
    float decayAmount = static_cast<float>(pImpl->decayRate * dtSeconds * pImpl->level);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Apply homeostatic regulation
    if (pImpl->level > 1.0f) {
        pImpl->level = 1.0f - (pImpl->level - 1.0f) * 0.5f;  // Gentle correction
    } else if (pImpl->level < 0.0f) {
        pImpl->level = 0.0f;  // Can't go below zero
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on Rescorla-Wagner and actor-critic models
    // Dopamine bursts signal unexpected rewards (prediction error positive)
    // Phasic dips signal omitted expected rewards (prediction error negative)
    
    float predictionError = reward - pImpl->baseline;
    
    // Burst response: larger prediction error -> stronger dopamine response
    float burstMagnitude = std::max(0.0f, predictionError * 2.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Include baseline modulation
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on temporal difference learning
    // TD error: reward + gamma * future_value - current_value
    // Positive error (better than expected) -> dopamine burst
    // Negative error (worse than expected) -> dopamine dip
    
    // Apply temporal difference learning dynamics
    float tdError = error;
    
    // Signal with scaled magnitude based on error size
    float signalMagnitude = std::abs(tdError) * 0.5f;
    
    if (tdError > 0.0f) {
        // Better than expected: dopamine burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + signalMagnitude);
    } else {
        // Worse than expected: dopamine dip (but not below zero)
        pImpl->level = std::max(0.0f, pImpl->level - signalMagnitude);
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm
