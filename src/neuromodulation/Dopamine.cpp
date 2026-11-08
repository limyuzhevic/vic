#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float accumulatedReward;
    std::vector<float> history;
    std::vector<float> rewardHistory;
    float lastSignalTime;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), accumulatedReward(0.0f), lastSignalTime(0.0f) {}
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
    // Real dopamine-modulated plasticity: dopamine enhances learning
    // Both positive and negative prediction errors modulate plasticity
    // High dopamine increases synaptic learning rate
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine decays towards baseline with exponential decay
    // This models the biological process of dopamine reuptake and clearance
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to unexpected rewards
    // Models the VTA (ventral tegmental area) firing increase
    float phasicResponse = std::min(reward, 1.0f) * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicResponse);
    pImpl->accumulatedReward += reward;
    pImpl->rewardHistory.push_back(reward);
    
    NLM_LOG_INFO("Dopamine reward signal: " + std::to_string(reward) + 
                 " (current level: " + std::to_string(pImpl->level) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error: dopamine encodes prediction errors
    // Positive error = unexpected reward (learning to approach)
    // Negative error = missed reward (learning to avoid)
    // This is the core reinforcement learning signal
    pImpl->predictionError = error;
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.5f);
    pImpl->history.push_back(pImpl->level);
    
    if (error > 0.0f) {
        NLM_LOG_INFO("Dopamine RPE positive: " + std::to_string(error) + 
                     " (increase in level: " + std::to_string(error * pImpl->releaseRate * 0.5f) + ")");
    } else {
        NLM_LOG_INFO("Dopamine RPE negative: " + std::to_string(error) + 
                     " (decrease in level: " + std::to_string(-error * pImpl->releaseRate * 0.5f) + ")");
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 0.0f, 2.0f);
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.0f, rate);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->history.clear();
    pImpl->rewardHistory.clear();
    NLM_LOG_INFO("Dopamine system reset");
}

float Dopamine::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

size_t Dopamine::getHistorySize() const {
    return pImpl->history.size();
}

} // namespace nlm