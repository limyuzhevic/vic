#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), plasticityFactor(1.0f) {}
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
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine dynamics: decay towards baseline, modulated by recent activity
    float timeFactor = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeFactor);
    
    // Update plasticity factor based on current level
    pImpl->plasticityFactor = 0.5f + 0.5f * pImpl->level;  // Higher dopamine = higher plasticity
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to unexpected reward
    if (reward > 0.0f) {
        // Burst of dopamine proportional to reward magnitude
        float burst = std::min(reward * pImpl->releaseRate, pImpl->peak - pImpl->level);
        pImpl->level += burst;
        
        // Temporarily increase plasticity
        pImpl->plasticityFactor = std::min(2.0f, pImpl->plasticityFactor + 0.3f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine encodes reward prediction error
    // Positive error: better than expected -> learning boost
    // Negative error: worse than expected -> learning suppression
    
    if (error > 0.0f) {
        // Positive prediction error: increase dopamine
        float boost = std::min(error * pImpl->releaseRate, pImpl->peak - pImpl->level);
        pImpl->level += boost;
        
        // Enhance plasticity for learning from positive prediction errors
        pImpl->plasticityFactor = std::min(2.0f, pImpl->plasticityFactor + 0.2f);
    } else if (error < 0.0f) {
        // Negative prediction error: decrease dopamine
        float decrease = std::min(std::abs(error) * pImpl->releaseRate * 0.5f, pImpl->level - pImpl->baseline);
        pImpl->level -= decrease;
        
        // Suppress plasticity for learning from negative prediction errors
        pImpl->plasticityFactor = std::max(0.1f, pImpl->plasticityFactor * 0.8f);
    }
}

} // namespace nlm
