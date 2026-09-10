#include "Dopamine.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float expectedReward;  // Track expected reward for prediction error
    float activitySum;     // For activity-dependent plasticity
    size_t recentActivityCount;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), expectedReward(0.0f), activitySum(0.0f), 
             recentActivityCount(0) {}
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()), expectedReward(0.0f), recentActivityCount(0) {}

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
    // Implement real dopamine-modulated plasticity factor
    // Factors: baseline plasticity, dopamine scaling, activity-dependent modulation
    float basePlasticity = 0.01f;
    float dopamineModulation = 0.8f * std::tanh(pImpl->level);
    
    // Activity-dependent plasticity: more plasticity when neurons are active
    float activityFactor = 1.0f + 0.5f * static_cast<float>(pImpl->recentActivityCount) / 100.0f;
    
    return basePlasticity * (1.0f + dopamineModulation * activityFactor);
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics with differential equations
    float timeStep = static_cast<float>(dt);
    
    // Decay toward baseline with biological variability
    float variability = ((static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f) * 0.01f;
    float decayChange = -pImpl->decayRate * (pImpl->level - pImpl->baseline) * timeStep;
    pImpl->level += decayChange + variability;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Reset activity counter for next time step
    pImpl->recentActivityCount = 0;
}

void Dopamine::signalReward(float reward) {
    // Implement reward prediction error signaling
    float predictionError = reward - pImpl->expectedReward;
    pImpl->expectedReward = reward;
    
    // Dopamine burst for positive prediction error (learning)
    if (predictionError > 0.0f) {
        pImpl->level = std::min(pImpl->peak, 
                               pImpl->level + pImpl->releaseRate * predictionError * 2.0f);
    }
    // Dopamine dip for negative prediction error (prediction update)
    else {
        pImpl->level = std::max(0.0f,
                               pImpl->level + pImpl->releaseRate * predictionError);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling (fast pathway)
    pImpl->level = std::max(0.0f,
                           pImpl->level + error * pImpl->releaseRate * 0.5f);
}

} // namespace nlm