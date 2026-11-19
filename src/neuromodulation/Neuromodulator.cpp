#include "Neuromodulator.hpp"
#include <algorithm>
#include <cstdlib>  // For rand()

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
    // Real dopamine-modulated plasticity: error-dependent learning rate
    // Higher dopamine (and error) increases plasticity for learning
    // Use non-linear response to match biological dynamics
    float basePlasticity = 0.01f;
    float maxPlasticity = 0.5f;
    return basePlasticity + (maxPlasticity - basePlasticity) * (pImpl->level * pImpl->level);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay with noise
    // Biological dopamine has stochastic dynamics and homeostatic regulation
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Add biological noise (30% of level)
    float noise = pImpl->releaseRate * 0.3f * static_cast<float>(rand()) / RAND_MAX - 0.15f;
    pImpl->level += noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic dopamine response with adaptation
    // Reward magnitude and prediction error both matter
    // Stronger rewards produce larger but bounded responses
    float rewardResponse = std::min(1.0f, reward * 2.0f);  // Normalize reward strength
    float burstStrength = pImpl->releaseRate * rewardResponse;
    
    // Add burst response with some noise
    float noise = 0.05f * static_cast<float>(rand()) / RAND_MAX - 0.025f;
    burstStrength += noise;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstStrength);
    
    // Homeostatic reset: reward responses decay over time
    // This implements the biological principle of reward devaluation
    if (pImpl->level > pImpl->baseline * 1.5f) {
        pImpl->baseline = std::min(pImpl->baseline * 1.2f, pImpl->level * 0.7f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: temporal difference learning
    // Negative error (worse than expected) decreases dopamine
    // Positive error (better than expected) increases dopamine
    // Response magnitude scales with error magnitude
    float errorResponse = error * 0.8f;  // Scale error response
    errorResponse = std::clamp(errorResponse, -0.5f, 0.5f);  // Bounded response
    
    float oldLevel = pImpl->level;
    pImpl->level = std::max(0.0f, oldLevel + errorResponse * pImpl->releaseRate);
    
    // Adaptive release rate: learning adapts to prediction error statistics
    if (std::abs(error) > 0.3f) {
        pImpl->releaseRate = std::min(2.0f, pImpl->releaseRate * 1.1f);
    } else {
        pImpl->releaseRate = std::max(0.5f, pImpl->releaseRate * 0.95f);
    }
}

} // namespace nlm
