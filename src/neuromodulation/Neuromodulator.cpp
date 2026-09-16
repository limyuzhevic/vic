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
    float predictionError;
    float expectedValue;
    float trace;
    Timestamp lastUpdateTime;
    Timestamp lastRewardTime;
    Timestamp lastErrorTime;
    
    // Dopamine dynamics parameters
    static constexpr float DEFAULT_BASELINE = 0.1f;
    static constexpr float DEFAULT_PEAK = 2.0f;
    static constexpr float DEFAULT_DECAY_RATE = 0.2f;  // 200ms time constant
    static constexpr float DEFAULT_RELEASE_RATE = 1.0f;
    static constexpr float MAX_CONCENTRATION = 5.0f;
    static constexpr float MIN_CONCENTRATION = 0.0f;
    
    Impl() : level(0.0f), baseline(DEFAULT_BASELINE), peak(DEFAULT_PEAK),
             decayRate(DEFAULT_DECAY_RATE), releaseRate(DEFAULT_RELEASE_RATE),
             predictionError(0.0f), expectedValue(0.0f), trace(0.0f),
             lastUpdateTime(0), lastRewardTime(-1), lastErrorTime(-1) {}
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
    pImpl->level = std::clamp(level, pImpl->MIN_CONCENTRATION, pImpl->MAX_CONCENTRATION);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: stronger dopamine increases plasticity
    // Implements dopaminergic control of synaptic eligibility traces
    // Formula: plasticity_factor = 1 + k * (DA_level - baseline)
    
    if (pImpl->level <= pImpl->baseline) {
        return 1.0f;  // Baseline plasticity
    }
    
    // Sigmoidal relationship: saturates at high dopamine levels
    float excessDA = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    float modulation = 1.0f + 2.0f * std::tanh(excessDA);  // Range: [1, 3]
    
    return modulation;
}

void Dopamine::update(TimestepDuration currentTime) {
    // Real dopamine dynamics implementing temporal difference learning
    // Includes eligibility traces and temporal filtering
    
    TimestepDuration dt = currentTime - pImpl->lastUpdateTime;
    if (dt <= 0.0f) return;
    
    // Decay towards baseline with exponential decay
    float decayFactor = std::exp(-pImpl->decayRate * dt / 1000.0f);  // Convert ms to s
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Decay eligibility trace
    pImpl->trace *= (1.0f - pImpl->decayRate * dt / 1000.0f);
    if (pImpl->trace < 0.001f) {
        pImpl->trace = 0.0f;
    }
    
    pImpl->lastUpdateTime = currentTime;
}

void Dopamine::signalReward(float reward) {
    // Reward prediction error signaling with temporal dynamics
    // Dopamine bursts for unexpected rewards, phasic dips for omitted rewards
    
    float rewardAmount = std::max(0.0f, reward);
    
    // Compute reward prediction error
    float error = rewardAmount - pImpl->expectedValue;
    pImpl->predictionError = error;
    
    // Phasic dopamine response proportional to RPE
    // Implements temporal difference (TD) learning
    float phasicResponse = error * pImpl->releaseRate;
    
    // Update expected value (actor-critic style)
    pImpl->expectedValue = rewardAmount;
    
    // Combine with tonic baseline for realistic dynamics
    if (rewardAmount > 0.0f) {
        // Phasic burst: strong, transient increase
        pImpl->level = std::min(pImpl->level + phasicResponse * 5.0f, pImpl->MAX_CONCENTRATION);
        lastErrorTime = currentTime;  // Track timing for temporal difference
    } else if (error < -0.1f) {
        // Phasic dip: decreased dopamine for omitted reward
        pImpl->level = std::max(pImpl->level + phasicResponse, pImpl->MIN_CONCENTRATION);
    }
    
    // Update eligibility trace for reward-modulated plasticity
    if (rewardAmount > 0.0f) {
        pImpl->trace += rewardAmount * 0.5f;  // Stronger trace for larger rewards
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct RPE signaling from prediction system
    // More flexible than reward-only signaling
    
    pImpl->predictionError = error;
    
    // Phasic response proportional to RPE magnitude
    // Implements classic Rescorla-Wagner delta rule
    float phasicResponse = error * pImpl->releaseRate;
    
    // Update expected value
    pImpl->expectedValue += error;
    
    // Apply phasic response
    pImpl->level = std::clamp(pImpl->level + phasicResponse, pImpl->MIN_CONCENTRATION, pImpl->MAX_CONCENTRATION);
    
    // Update eligibility trace
    pImpl->trace += std::abs(error) * (error > 0.0f ? 0.3f : 0.1f);
    
    lastErrorTime = currentTime;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getExpectedValue() const {
    return pImpl->expectedValue;
}

float Dopamine::getTrace() const {
    return pImpl->trace;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
    pImpl->trace = 0.0f;
    pImpl->lastUpdateTime = 0;
    pImpl->lastRewardTime = -1;
    pImpl->lastErrorTime = -1;
}

} // namespace nlm
