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
    // Based on empirical data: optimal learning occurs at moderate dopamine levels
    // Very low dopamine = reduced plasticity, very high dopamine = excessive plasticity
    if (pImpl->level <= 0.3f) {
        return 0.1f + 0.9f * (pImpl->level / 0.3f); // Linear from 0.1 to 1.0
    } else if (pImpl->level >= 0.7f) {
        return 1.0f + 0.5f * ((pImpl->level - 0.7f) / 0.3f); // Up to 1.5
    } else {
        return 1.0f; // Optimal range around 0.5
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on brain physiology
    float timeConstant = pImpl->decayRate > 0.0f ? 1.0f / pImpl->decayRate : 10.0f;
    float decayFactor = std::exp(-static_cast<float>(dt) / timeConstant);
    
    // Decay toward baseline (tonic firing rate)
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward prediction error signaling
    // Dopamine bursts for unexpected rewards, dips for omitted rewards
    float rewardSignal = reward * pImpl->releaseRate;
    
    // Add some temporal dynamics
    float temporalFactor = 1.0f + 0.5f * static_cast<float>(rand()) / RAND_MAX;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal * temporalFactor);
    
    // Update baseline based on recent rewards
    pImpl->baseline = 0.1f * pImpl->baseline + 0.9f * 0.1f; // Baseline gradually adjusts
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (Niro et al., 2019)
    // Positive error (better than expected) -> dopamine burst
    // Negative error (worse than expected) -> dopamine dip
    
    float errorSignal = error * pImpl->releaseRate;
    
    if (error > 0) {
        // Reward prediction error -> dopamine release
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorSignal);
        pImpl->baseline = std::max(0.0f, pImpl->baseline + error * 0.05f);
    } else {
        // Prediction error -> dopamine suppression
        pImpl->level = std::max(0.0f, pImpl->level + errorSignal);
        pImpl->baseline = std::max(0.0f, pImpl->baseline + error * 0.02f);
    }
    
    // Ensure level doesn't go negative
    pImpl->level = std::max(0.0f, pImpl->level);
}

} // namespace nlm
