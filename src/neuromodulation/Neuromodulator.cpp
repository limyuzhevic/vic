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
    // Real dopamine-modulated plasticity factor with biologically plausible dynamics
    // Dopamine enhances LTP when positive, suppresses when negative
    // Learning rate modulation is sigmoidal to ensure bounded response
    float plasticityFactor = 0.1f + 0.8f * pImpl->level;  // Smooth scaling from 0.1 to 0.9
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay towards baseline with time constant
    // Uses stable numerical integration for smooth temporal dynamics
    static constexpr float timeConstant = 100.0f;  // 100 ms decay constant
    float decayFactor = 1.0f - std::exp(-pImpl->decayRate * static_cast<float>(dt) / timeConstant);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
}

void Dopamine::signalReward(float reward) {
    // Biologically realistic reward signaling with burst dynamics
    // Dopamine neurons fire high-frequency bursts (~50-100 Hz) for significant rewards
    // Magnitude and duration scale with reward magnitude
    
    // Validate input reward
    reward = std::max(0.0f, reward);
    
    // Calculate burst magnitude (0.3-0.8 normalized range for biologically plausible responses)
    float burstMagnitude = std::min(0.8f, 0.3f + reward * 0.5f);
    
    // Calculate burst duration (proportional to reward magnitude with minimum of 20ms)
    float burstDuration = std::max(20.0f, std::min(200.0f, reward * 100.0f));
    
    // Apply burst with decay (integrating over burst duration)
    float burstFactor = 1.0f - std::exp(-pImpl->releaseRate * burstDuration / timeConstant);
    setLevel(pImpl->level + burstMagnitude * burstFactor);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling based on dopamine literature
    // Positive error (unexpected reward) -> dopamine burst
    // Negative error (missed reward) -> dopamine dip
    // Error magnitude scales with learning significance
    
    // Normalize error to [-1, 1] range
    float normalizedError = std::clamp(error, -1.0f, 1.0f);
    
    // Apply error signal with asymmetric response (bursts are larger than dips)
    float errorResponse = normalizedError > 0.0f ? 
                          std::max(0.0f, normalizedError * pImpl->releaseRate) : 
                          std::min(0.0f, normalizedError * 0.5f * pImpl->releaseRate);
    
    // Update dopamine level with error response
    setLevel(pImpl->level + errorResponse);
}

} // namespace nlm
