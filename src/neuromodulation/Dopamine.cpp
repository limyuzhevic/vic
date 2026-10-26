#include "Dopamine.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedValue;
    float learningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionError(0.0f), expectedValue(0.0f), learningRate(0.1f) {}
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
    // Higher dopamine increases learning rate for LTP, decreases for LTD
    float learningRate = 1.0f;
    if (pImpl->level > 0.7f) {
        // Strong reward -> high learning rate
        learningRate = 1.5f;
    } else if (pImpl->level < 0.3f) {
        // Low/no reward -> low learning rate
        learningRate = 0.5f;
    } else {
        // Moderate reward -> normal learning rate
        learningRate = 1.0f;
    }
    
    return learningRate;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with prediction error
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update expected value with decay
    pImpl->expectedValue *= 0.95f;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal difference learning
    float rewardPredictionError = reward - pImpl->expectedValue;
    
    // Update expected value (TD learning)
    pImpl->expectedValue += rewardPredictionError * pImpl->learningRate;
    
    // Dopamine burst proportional to prediction error
    float burst = std::max(0.0f, rewardPredictionError * 2.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Error is the difference between expected and actual reward
    float predictionError = error;
    
    // Update expected value
    pImpl->expectedValue += predictionError * pImpl->learningRate;
    
    // Dopamine response to prediction error
    if (predictionError > 0.0f) {
        // Positive prediction error (better than expected) -> dopamine burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate * 1.5f);
    } else if (predictionError < 0.0f) {
        // Negative prediction error (worse than expected) -> dopamine dip
        pImpl->level = std::max(pImpl->baseline, pImpl->level + predictionError * 0.5f * pImpl->releaseRate);
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::setExpectedValue(float value) {
    pImpl->expectedValue = value;
}

float Dopamine::getExpectedValue() const {
    return pImpl->expectedValue;
}

} // namespace nlm