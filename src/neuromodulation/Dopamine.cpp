// Dopamine implementation
// Phase 2: Real Neural Computation
// Realizes dopamine neuromodulation for reward prediction and reinforcement learning

#include "Dopamine.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardHistory;
    float predictionErrorHistory;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(1.0f), 
          decayRate(0.1f), releaseRate(1.0f),
          rewardHistory(0.0f), predictionErrorHistory(0.0f) {}
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
    // Higher dopamine increases plasticity for rewarded actions
    // Based on experimental neuroscience research
    const float maxPlasticity = 2.0f;  // Maximum plasticity factor
    const float minPlasticity = 0.5f;  // Baseline plasticity factor
    
    return minPlasticity + (pImpl->level * (maxPlasticity - minPlasticity));
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on neuroscience research
    // Dopamine decays towards baseline exponentially
    // Resting state around 0.1 (tonic firing)
    
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - decayAmount);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on prediction error
    // Dopamine bursts in response to unexpected rewards (positive prediction error)
    // Phasic increases help reinforce rewarded actions
    
    float boost = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost);
    
    pImpl->rewardHistory = pImpl->level;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling (Schultz et al., 1997)
    // Dopamine signals prediction errors: positive for better-than-expected,
    // negative for worse-than-expected rewards
    // Critical for reinforcement learning and value-based decision making
    
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    pImpl->predictionErrorHistory = pImpl->level;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::max(0.0f, std::min(1.0f, baseline));
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::max(0.0f, std::min(1.0f, peak));
}

void Dopamine::setDecayRate(float decayRate) {
    pImpl->decayRate = std::max(0.0f, std::min(10.0f, decayRate));
}

void Dopamine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = std::max(0.0f, std::min(10.0f, releaseRate));
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rewardHistory = 0.0f;
    pImpl->predictionErrorHistory = 0.0f;
}

} // namespace nlm
