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
    // Enhanced dopamine-modulated plasticity factor
    // Incorporates temporal dynamics and concentration-dependent effects
    float temporalMod = 1.0f - std::exp(-pImpl->level / 0.3f); // Saturation curve
    float baselineOffset = pImpl->level > pImpl->baseline ? 1.0f : 0.0f;
    return baselineOffset + temporalMod * 0.8f;
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics with temporal filtering
    float decay = pImpl->decayRate * static_cast<float>(dt);
    float target = pImpl->baseline;
    
    // Smooth exponential decay toward baseline
    float diff = pImpl->level - target;
    if (diff > 0.0f) {
        pImpl->level = std::max(target, pImpl->level - decay * (1.0f + pImpl->level * 0.5f));
    } else {
        pImpl->level = std::min(target, pImpl->level + decay * 0.5f);
    }
    
    // Ensure bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling with temporal dynamics
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_WARNING("Invalid reward value: " + std::to_string(reward));
        reward = 0.0f;
    }
    
    // Constrained reward response
    float rewardResponse = std::clamp(reward, 0.0f, 2.0f);
    
    // Time-dependent response (burst effect)
    float burstFactor = 1.5f * std::exp(-pImpl->level * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse * pImpl->releaseRate * burstFactor);
    
    NLM_LOG_INFO("Dopamine reward response: " + std::to_string(reward) + 
                 " -> level: " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (more realistic implementation)
    if (std::isnan(error) || std::isinf(error)) {
        NLM_LOG_WARNING("Invalid prediction error value: " + std::to_string(error));
        error = 0.0f;
    }
    
    // RPE has asymmetric effects (positive for better-than-expected, negative for worse)
    float errorEffect = std::clamp(error, -1.0f, 2.0f);
    
    // Integration with current level
    float newLevel = pImpl->level + errorEffect * pImpl->releaseRate * (1.0f - pImpl->level * 0.2f);
    pImpl->level = std::clamp(newLevel, 0.0f, pImpl->peak);
    
    NLM_LOG_INFO("Dopamine RPE response: " + std::to_string(error) + 
                 " -> level: " + std::to_string(pImpl->level));
}

} // namespace nlm
