#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float predictionErrorTarget;
    float rewardHistory;
    size_t rewardHistorySize;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             adaptationRate(0.01f), predictionErrorTarget(0.1f), rewardHistory(0.0f), rewardHistorySize(10) {}
};

Dopamine::Dopamine(std::shared_ptr<Config> config) : pImpl(new Impl) {
    // Initialize dopamine from config
    if (config) {
        pImpl->baseline = config->getOr<float>("dopamine_baseline", 0.0f);
        pImpl->decayRate = config->getOr<float>("dopamine_decay_rate", 0.05f);
        pImpl->releaseRate = config->getOr<float>("dopamine_release_rate", 1.0f);
        pImpl->adaptationRate = config->getOr<float>("dopamine_adaptation_rate", 0.01f);
    }
}

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
    // Higher dopamine increases plasticity up to a maximum
    float basePlasticity = 0.5f;
    float modulation = pImpl->level * 0.5f;
    
    // Adapt plasticity based on prediction error
    float predictionErrorMod = pImpl->level * (1.0f - pImpl->confidence);
    
    // Dopamine also affects exploration-exploitation balance
    return basePlasticity + modulation + predictionErrorMod;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with adaptation and prediction error integration
    float timeDecay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - timeDecay);
    
    // Adapt baseline based on reward history
    pImpl->baseline = pImpl->baseline + pImpl->adaptationRate * (pImpl->rewardHistory - pImpl->baseline);
    
    // Update reward history (would normally come from reward prediction error)
    pImpl->rewardHistory = std::min(pImpl->rewardHistory + 0.01f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with prediction error integration
    // Dopamine bursts for unexpected rewards
    float rewardStrength = std::clamp(reward, 0.0f, 1.0f);
    
    // Signal strength is modulated by prediction error
    float predictionError = 1.0f - std::abs(reward - pImpl->predictionErrorTarget);
    float signalStrength = rewardStrength * predictionError * pImpl->releaseRate;
    
    // Update level with burst response
    pImpl->level = std::min(pImpl->peak, pImpl->level + signalStrength);
    
    // Update reward history for baseline adaptation
    pImpl->rewardHistory = pImpl->rewardHistory * 0.9f + rewardStrength * 0.1f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Dopamine encodes reward prediction error (RPE): actual - predicted reward
    float rpe = error; // Actual RPE signal
    
    // Stronger response for larger prediction errors
    float responseStrength = std::clamp(rpe * pImpl->releaseRate, -0.5f, 0.5f);
    
    // Update dopamine level with prediction error signal
    pImpl->level = std::max(0.0f, std::min(pImpl->level + responseStrength, pImpl->peak));
    
    // Adapt prediction error target based on recent experiences
    pImpl->predictionErrorTarget = pImpl->predictionErrorTarget * 0.99f + rpe * 0.01f;
}

} // namespace nlm
