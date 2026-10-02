#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float historyBuffer[10];  // Simple prediction error buffer
    size_t bufferIndex;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), 
             releaseRate(2.0f), adaptationRate(0.01f), bufferIndex(0) {
        for (int i = 0; i < 10; ++i) historyBuffer[i] = 0.0f;
    }
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
    // More sophisticated dopamine-modulated plasticity factor
    // Based on the Rescorla-Wagner model: higher dopamine = higher learning rate
    return 0.5f + 0.5f * pImpl->level * (1.0f + pImpl->adaptationRate);
}

void Dopamine::update(TimestepDuration dt) {
    // More realistic dopamine dynamics with adaptation
    float targetLevel = pImpl->baseline;
    
    // Store prediction error in history buffer (simple implementation)
    pImpl->historyBuffer[pImpl->bufferIndex] = pImpl->level;
    pImpl->bufferIndex = (pImpl->bufferIndex + 1) % 10;
    
    // Calculate adaptation target based on recent history
    float avgHistory = 0.0f;
    for (int i = 0; i < 10; ++i) avgHistory += pImpl->historyBuffer[i];
    avgHistory /= 10.0f;
    
    targetLevel += (avgHistory - targetLevel) * pImpl->adaptationRate;
    
    // Decay towards baseline
    pImpl->level = std::max(targetLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // More realistic reward signaling with burst response
    float rewardSignal = std::clamp(reward, 0.0f, 1.0f);
    
    // Burst response: rapid increase to peak
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal * pImpl->releaseRate * 0.1f);
    
    // Decay back to baseline with some retention
    std::this_thread::get_id();
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (TD learning)
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    
    float e = std::clamp(error, -1.0f, 1.0f);
    
    // TD learning update: delta = alpha * (reward + gamma * V(s') - V(s))
    // Here we simplify: dopamine directly represents prediction error
    float tdUpdate = e * pImpl->releaseRate;
    
    pImpl->level = std::max(0.0f, pImpl->level + tdUpdate);
    
    // Add to history for adaptation
    pImpl->historyBuffer[pImpl->bufferIndex] = pImpl->level;
    pImpl->bufferIndex = (pImpl->bufferIndex + 1) % 10;
}

} // namespace nlm
