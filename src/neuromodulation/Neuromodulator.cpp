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
    // IMPROVEMENT: Implement real dopamine-modulated plasticity factor
    // Based on biological research: Dopamine affects synaptic strength
    
    // Inverted U-curve relationship: moderate dopamine optimal for learning
    // High dopamine can impair plasticity, low dopamine reduces learning
    float normalizedLevel = std::clamp(pImpl->level / pImpl->peak, 0.0f, 1.0f);
    float optimalLevel = 0.6f; // Optimal dopamine level for plasticity
    float deviation = std::abs(normalizedLevel - optimalLevel);
    
    // Inverted U: high at optimal, low at extremes
    return std::max(0.1f, 1.0f - deviation * 1.5f);
}

void Dopamine::update(TimestepDuration dt) {
    // IMPROVEMENT: Implement real dopamine dynamics
    // Biological-based dopamine dynamics with temporal integration
    
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    float newLevel = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Add small stochastic noise for biological realism
    float noise = ((static_cast<float>(rand()) / RAND_MAX) * 0.02f - 0.01f) * pImpl->decayRate * static_cast<float>(dt);
    newLevel += noise;
    
    pImpl->level = std::clamp(newLevel, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // IMPROVEMENT: Implement real reward signaling
    // Dopamine burst response to unexpected rewards (prediction error)
    
    // Reward prediction error: larger than expected reward = bigger burst
    float predictionError = std::max(0.0f, reward - pImpl->baseline);
    
    // Burst response: saturate at peak with exponential rise
    float burstStrength = 1.0f - std::exp(-pImpl->releaseRate * predictionError);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstStrength * predictionError);
}

void Dopamine::signalRewardPredictionError(float error) {
    // IMPROVEMENT: Implement reward prediction error signaling
    // Negative prediction error (worse than expected) reduces dopamine
    // Positive prediction error (better than expected) increases dopamine
    
    // Exponential response to prediction error
    float response = 1.0f - std::exp(-pImpl->releaseRate * std::abs(error));
    float delta = (error > 0.0f) ? response : -response * 0.5f; // Asymmetric response
    
    pImpl->level = std::clamp(pImpl->level + delta, 0.0f, pImpl->peak);
}

} // namespace nlm
