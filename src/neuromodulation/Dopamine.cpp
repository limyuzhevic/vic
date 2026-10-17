#include "Dopamine.hpp"
#include <algorithm>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardHistory;
    float predictionErrorHistory;
    std::vector<float> recentRewards;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             rewardHistory(0.0f), predictionErrorHistory(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized");
}

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
    // Higher dopamine increases plasticity up to a point
    // With a sigmoidal relationship: factor = 0.5 + 0.5 * level
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update recent reward history
    if (pImpl->recentRewards.size() > 10) {
        pImpl->recentRewards.erase(pImpl->recentRewards.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward reception
    // Modulates with reward magnitude
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->rewardHistory = reward;
    pImpl->recentRewards.push_back(reward);
    
    NLM_LOG_DEBUG("Dopamine signal: reward " + std::to_string(reward) + ", level: " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to reward prediction error
    // Positive error (unexpected reward) -> increase dopamine
    // Negative error (expected reward not received) -> decrease dopamine
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->predictionErrorHistory = error;
    pImpl->recentRewards.push_back(error);
    
    NLM_LOG_DEBUG("Dopamine signal: prediction error " + std::to_string(error) + ", level: " + std::to_string(pImpl->level));
}

float Dopamine::getTotalSignal() const {
    // Combine reward and prediction error signals
    return (pImpl->rewardHistory + std::abs(pImpl->predictionErrorHistory)) / 2.0f;
}

const std::vector<float>& Dopamine::getRecentRewards() const {
    return pImpl->recentRewards;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rewardHistory = 0.0f;
    pImpl->predictionErrorHistory = 0.0f;
    pImpl->recentRewards.clear();
}

} // namespace nlm