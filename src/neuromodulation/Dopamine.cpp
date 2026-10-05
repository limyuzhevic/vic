#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorBias;
    float timeConstant;
    float adaptationVariable;
    Timestamp lastRewardTime;
    float rewardHistory[10];  // Sliding window of recent rewards
    size_t historyIndex;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f),
             predictionErrorBias(0.0f), timeConstant(1000.0f), adaptationVariable(0.0f),
             lastRewardTime(0.0f), historyIndex(0) {
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
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
    // Real dopamine-modulated plasticity factor based on Rescorla-Wagner model
    // Dopamine encodes reward prediction error (RPE)
    // Plasticity = baseline + RPE * gain
    float rpe = calculateRewardPredictionError();
    float gain = 2.0f;  // Dopamine sensitivity
    return 0.1f + gain * rpe;  // Baseline 0.1, modulatory range +/- 2.0
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics based on behavioral neuroscience
    // Two main components: tonic (baseline) and phasic (burst) activity
    
    // Tonic component: decay towards baseline
    float tonicDecay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - tonicDecay);
    
    // Adaptation component: slow negative feedback
    pImpl->adaptationVariable *= 0.99f;  // Decay adaptation variable
    pImpl->level *= (1.0f - pImpl->adaptationVariable * 0.1f);
    
    // Update reward history (sliding window)
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on phasic dopamine bursts
    // Dopamine neurons show burst firing (~100ms) when reward exceeds expectation
    
    // Calculate reward prediction error
    float rpe = calculateRewardPredictionError();
    
    // Phasic response: scaled by RPE magnitude and sign
    float phasicResponse = reward * 0.5f + rpe * 2.0f;
    
    // Ensure we don't exceed peak level
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicResponse * pImpl->releaseRate * 0.1f);
    
    // Record reward for RPE calculation
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->lastRewardTime = 0.0f;  // Reset time since last reward
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct RPE signaling without external reward
    // Used when prediction error signals are available externally
    
    pImpl->predictionErrorBias = error;
    
    // Phasic response to prediction error
    float response = error * pImpl->releaseRate * 0.5f;
    pImpl->level = std::clamp(pImpl->level + response, pImpl->baseline, pImpl->peak);
}

float Dopamine::calculateRewardPredictionError() const {
    // Calculate reward prediction error (RPE) using temporal difference learning
    // RPE = reward + gamma * V(t+1) - V(t)
    
    if (pImpl->historyIndex == 0 && std::all_of(std::begin(pImpl->rewardHistory), std::end(pImpl->rewardHistory),
                                               [](float r) { return r == 0.0f; })) {
        return 0.0f;  // No rewards yet
    }
    
    // Simple RPE calculation: reward - prediction
    float currentPrediction = pImpl->baseline + pImpl->predictionErrorBias * 0.5f;
    float recentReward = pImpl->rewardHistory[pImpl->historyIndex];
    
    float rpe = recentReward - currentPrediction;
    return std::clamp(rpe, -1.0f, 1.0f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->adaptationVariable = 0.0f;
    pImpl->predictionErrorBias = 0.0f;
    std::fill(std::begin(pImpl->rewardHistory), std::end(pImpl->rewardHistory), 0.0f);
    pImpl->historyIndex = 0;
    pImpl->lastRewardTime = 0.0f;
    
    // Validate reset state
    if (std::isnan(pImpl->level) || std::isinf(pImpl->level)) {
        NLM_LOG_ERROR("Invalid state after dopamine reset: " + std::to_string(pImpl->level));
        pImpl->level = pImpl->baseline;  // Force recovery
    }
}

} // namespace nlm