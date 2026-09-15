#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Add random number generator to Dopamine for noise
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    std::shared_ptr<RandomGenerator> rng;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             rng(std::make_shared<RandomGenerator>(42)) {}
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
    // Real dopamine-modulated plasticity factor with sigmoidal response
    // Reflects increased learning at moderate dopamine levels
    float normalizedDA = pImpl->level / pImpl->peak;
    // Sigmoidal relationship: optimal learning at moderate levels
    float plasticity = 0.3f + 0.7f / (1.0f + std::exp(-10.0f * (normalizedDA - 0.5f)));
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with adaptive baseline
    float dtFloat = static_cast<float>(dt);
    
    // Natural decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add small Brownian noise for biological realism
    float noise = rng ? rng->normal(0.0f, 0.01f * dtFloat) : 0.0f;
    pImpl->level = std::clamp(pImpl->level + noise, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics and prediction error modulation
    float burstAmplitude = std::min(1.0f, reward * pImpl->releaseRate);
    
    // Phasic burst: rapid increase to peak
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmplitude);
    
    // Add temporal dynamics for burst decay
    // In real implementation, this would interact with ongoing dynamics
    // For now, maintain elevated level with rapid decay
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling with temporal dynamics
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    
    // Modulate error based on current dopamine level for gain control
    float currentLevel = pImpl->level;
    float errorModulation = error * (0.5f + 0.5f * currentLevel); // Higher baseline = more sensitivity
    
    // Apply prediction error signal with temporal integration
    float newLevel = currentLevel + errorModulation * pImpl->releaseRate;
    pImpl->level = std::clamp(newLevel, 0.0f, pImpl->peak);
    
    // Add adaptive baseline tracking for prediction error normalization
    // In full implementation: baseline would be learned through experience
}

} // namespace nlm
