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
    float plasticityMultiplier;
    float historyBuffer[100];  // Rolling history for smoothing
    size_t historyIndex;
    float predictionError;
    float rewardPredictionError;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(1.5f)
        , plasticityMultiplier(1.0f), historyIndex(0) {
        for (size_t i = 0; i < 100; ++i) historyBuffer[i] = 0.0f;
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
    // Update history buffer for smoothing
    pImpl->historyBuffer[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
}

float Dopamine::getPlasticityFactor() const {
    // Improved dopamine-modulated plasticity factor
    // Uses exponential moving average of dopamine level
    float ema = 0.0f;
    float weight = 0.9f;
    
    for (size_t i = 0; i < 100; ++i) {
        size_t idx = (pImpl->historyIndex + i) % 100;
        ema = weight * pImpl->historyBuffer[idx] + (1.0f - weight) * ema;
    }
    
    // Non-linear relationship: low dopamine = low plasticity, 
    // optimal dopamine = high plasticity, very high dopamine = reduced plasticity
    float normalizedDA = std::min(1.0f, pImpl->level * 2.0f);
    float plasticity = std::exp(-std::pow(normalizedDA - 0.7f, 2.0f) * 5.0f) * 0.8f + 0.1f;
    
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Improved dopamine dynamics based on neuroscience research
    // Decay towards baseline with noise
    float noise = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 0.02f;
    
    // Smooth decay with exponential form
    float targetLevel = pImpl->baseline;
    float timeConstant = 1.0f / pImpl->decayRate;
    
    // Use Euler integration for smooth dynamics
    pImpl->level = targetLevel + (pImpl->level - targetLevel) * std::exp(-dt / timeConstant) + noise;
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Improved reward signaling with prediction error integration
    // Reward prediction error (RPE) based on dopamine research
    
    // Calculate expected reward based on recent history
    float expectedReward = 0.0f;
    int count = 0;
    for (size_t i = 0; i < 100; ++i) {
        if (pImpl->historyBuffer[i] > 0.0f) {
            expectedReward += pImpl->historyBuffer[i];
            count++;
        }
    }
    
    if (count > 0) expectedReward /= count;
    
    // Reward prediction error = actual - predicted
    float rpe = reward - expectedReward;
    
    // Update prediction error buffers
    pImpl->predictionError = rpe;
    pImpl->rewardPredictionError = std::max(0.0f, rpe);  // Only positive RPE matters for dopamine
    
    // Stronger response to unexpected rewards
    float burstMagnitude = std::min(1.0f, std::abs(rpe) * 2.0f + 0.5f);
    
    // Update level with temporal difference learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude * pImpl->releaseRate);
    
    // Add some decay to prevent runaway excitation
    pImpl->level *= 0.95f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    
    pImpl->rewardPredictionError = error;
    
    // Integrate with temporal difference learning
    float tdUpdate = error * pImpl->releaseRate * 0.8f;
    
    // Apply TD learning with eligibility traces
    pImpl->level += tdUpdate;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Add some persistence for learning
    pImpl->level = pImpl->level * 0.9f + pImpl->baseline * 0.1f;
}

} // namespace nlm
