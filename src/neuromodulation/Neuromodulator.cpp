#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeConstant;
    
    // Reward prediction error components
    float rewardPredictionError;
    float temporalDifference;
    float reward; // Current reward signal
    
    // History
    std::vector<float> history;
    
    // Biological parameters
    float synthesisRate;      // Rate of dopamine synthesis
    float reuptakeRate;       // Rate of dopamine reuptake
    float volumeOfDistribution; // How widely dopamine spreads
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.2f), 
             releaseRate(0.8f), timeConstant(0.1f), rewardPredictionError(0.0f),
             temporalDifference(0.0f), reward(0.0f), synthesisRate(0.5f),
             reuptakeRate(0.3f), volumeOfDistribution(0.8f) {}
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Biological dopamine-plasticiteit relationship
    // Dopamine enhances LTP and suppresses LTD
    // Higher dopamine -> more LTP, less LTD
    if (pImpl->level < 0.1f) {
        return 0.8f + 0.2f * pImpl->level / 0.1f; // Low dopamine: moderate plasticity
    } else {
        return 1.0f + 0.5f * (pImpl->level - 0.1f) / 0.9f; // High dopamine: enhanced plasticity
    }
}

void Dopamine::update(TimestepDuration dt, const std::unordered_map<std::string, float>& inputs) {
    float dtSeconds = static_cast<float>(dt) * 0.001f; // Convert to seconds
    
    // Get inputs
    auto it = inputs.find("reward_prediction_error");
    if (it != inputs.end()) {
        pImpl->rewardPredictionError = it->second;
    }
    
    it = inputs.find("temporal_difference");
    if (it != inputs.end()) {
        pImpl->temporalDifference = it->second;
    }
    
    it = inputs.find("reward");
    if (it != inputs.end()) {
        pImpl->reward = it->second;
    }
    
    // Reward prediction error (RPE) - key dopamine signal
    // Based on temporal difference learning: RPE = reward + gamma * V(s') - V(s)
    float rpe = pImpl->rewardPredictionError;
    
    // Apply RPE to dopamine level with temporal dynamics
    // Dopamine responds to unexpected rewards (positive RPE) and aversive outcomes (negative RPE)
    float targetLevel = pImpl->baseline + rpe * pImpl->releaseRate;
    
    // Smooth transition to target level (integration)
    float alpha = 1.0f - std::exp(-dtSeconds / pImpl->timeConstant);
    pImpl->level = pImpl->level * (1.0f - alpha) + targetLevel * alpha;
    
    // Natural decay toward baseline
    pImpl->level = pImpl->level - pImpl->decayRate * pImpl->level * dtSeconds;
    
    // Bounded by peak level
    pImpl->level = std::min(pImpl->level, pImpl->peak);
    
    // Ensure non-negative
    pImpl->level = std::max(0.0f, pImpl->level);
    
    // Add to history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) { // Keep last 1000 samples
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Dopamine::signalReward(float reward, float rewardPrediction) {
    // Reward signaling - immediate burst of dopamine
    float rpe = reward - rewardPrediction;
    
    // Burst response: immediate increase in dopamine level
    pImpl->rewardPredictionError = rpe;
    pImpl->reward = reward;
    
    // Strong response to unexpected rewards
    float burstLevel = pImpl->level + rpe * pImpl->releaseRate * 2.0f;
    pImpl->level = std::min(burstLevel, pImpl->peak);
    
    // History
    pImpl->history.push_back(pImpl->level);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    pImpl->rewardPredictionError = error;
    pImpl->temporalDifference = error;
    
    // Modulate dopamine level based on RPE
    float modulation = error * pImpl->releaseRate;
    pImpl->level += modulation;
    pImpl->level = std::max(0.0f, std::min(pImpl->level, pImpl->peak));
    
    // History
    pImpl->history.push_back(pImpl->level);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.5f);
}

void Dopamine::setPeakLevel(float peak) {
    pImpl->peak = std::max(0.1f, peak);
}

void Dopamine::setDecayRate(float decayRate) {
    pImpl->decayRate = std::max(0.01f, decayRate);
}

void Dopamine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = std::max(0.01f, releaseRate);
}

void Dopamine::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(0.001f, tau);
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->temporalDifference = 0.0f;
    pImpl->reward = 0.0f;
    pImpl->history.clear();
}

float Dopamine::getPredictionError() const {
    return pImpl->rewardPredictionError;
}

float Dopamine::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

} // namespace nlm
