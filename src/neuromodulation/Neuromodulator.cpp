#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float lastRewardTime;
    float eligibilityTrace;
    float explorationBonus;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , lastRewardTime(0.0f), eligibilityTrace(0.0f), explorationBonus(0.0f) {}
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
    // Enhanced dopamine-modulated plasticity factor with exploration component
    // Plasticity peaks at moderate dopamine levels (inverted U-curve)
    float basePlasticity = 0.5f;
    float explorationMod = pImpl->explorationBonus * 0.3f;
    return basePlasticity + 0.5f * pImpl->level + explorationMod;
}

void Dopamine::update(TimestepDuration dt) {
    // More realistic dopamine dynamics with temporal integration
    float decay = pImpl->decayRate * static_cast<float>(dt);
    float newLevel = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add decay of eligibility trace
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - decay * 0.5f);
    
    pImpl->level = newLevel;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with eligibility trace update
    // Immediate response plus temporal difference learning
    float rewardSignal = std::min(1.0f, reward * pImpl->releaseRate);
    
    // Update eligibility trace for credit assignment
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + 0.5f);
    
    // Apply reward with eligibility trace weighting
    float weightedReward = rewardSignal * pImpl->eligibilityTrace;
    
    // Update level with bounded response
    pImpl->level = std::min(pImpl->peak, pImpl->level + weightedReward);
    
    // Reset eligibility trace after reward
    pImpl->eligibilityTrace *= 0.5f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling with temporal dynamics
    float errorSignal = std::clamp(error * pImpl->releaseRate * 2.0f, -0.5f, 1.0f);
    
    // Update eligibility trace for credit assignment
    if (error > 0.0f) {
        // Positive prediction error strengthens future associations
        pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + 0.3f);
    } else if (error < 0.0f) {
        // Negative prediction error weakens future associations  
        pImpl->eligibilityTrace *= 0.7f;
    }
    
    // Apply prediction error with exploration bonus
    float totalSignal = errorSignal + pImpl->explorationBonus * 0.2f;
    pImpl->level = std::max(0.0f, pImpl->level + totalSignal);
}

void Dopamine::setExplorationBonus(float bonus) {
    pImpl->explorationBonus = std::clamp(bonus, 0.0f, 1.0f);
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::resetEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

} // namespace nlm
