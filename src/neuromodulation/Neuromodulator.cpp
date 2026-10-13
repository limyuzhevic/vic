#include "Neuromodulator.hpp"
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
    float targetLevel;
    float adaptRate;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , predictionError(0.0f)
        , targetLevel(0.0f)
        , adaptRate(0.05f) {}
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
    pImpl->targetLevel = pImpl->level;
    NLM_LOG_DEBUG(std::string("Dopamine level set to ") + std::to_string(level));
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Uses sigmoidal function to map dopamine level to plasticity range
    // Higher dopamine increases both LTP and LTD efficiency
    float normalizedDA = std::tanh(pImpl->level * 3.0f); // Scale for better response
    float plasticity = 0.2f + 0.8f * normalizedDA;  // Range: [0.2, 1.0]
    
    // Apply prediction error modulation
    float peModulation = std::exp(-std::abs(pImpl->predictionError) * 0.5f);
    plasticity *= peModulation;
    
    NLM_LOG_DEBUG(std::string("Dopamine plasticity factor: ") + std::to_string(plasticity) + 
                 " (DA: " + std::to_string(pImpl->level) + 
                 ", PE: " + std::to_string(pImpl->predictionError) + ")");
    
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Adaptive dynamics: smoothly approach target level
    if (std::abs(pImpl->level - pImpl->targetLevel) > 0.01f) {
        float adapt = pImpl->adaptRate * deltaTime;
        pImpl->level += (pImpl->targetLevel - pImpl->level) * adapt;
    }
    
    // Decay towards baseline
    float decay = pImpl->decayRate * deltaTime;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Store prediction error
    pImpl->predictionError = 0.0f;
    
    if (pImpl->level < 0.1f) {
        NLM_LOG_TRACE("Dopamine at baseline level (reward prediction)");
    } else if (pImpl->level > 0.8f) {
        NLM_LOG_TRACE("Dopamine peak level (reward received)");
    }
}

void Dopamine::signalReward(float reward) {
    NLM_LOG_INFO(std::string("Dopamine: Reward signal received: ") + std::to_string(reward));
    
    // Strong burst response to reward
    float burst = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    pImpl->targetLevel = burst;
    
    // Dopamine release adapts to reward magnitude
    if (reward > 0.5f) {
        // Large reward: strong dopamine burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f);
    } else if (reward > 0.1f) {
        // Small reward: moderate dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    NLM_LOG_INFO(std::string("Dopamine: Reward prediction error: ") + std::to_string(error));
    
    pImpl->predictionError = error;
    
    // Phasic dopamine response to prediction error
    if (error > 0.0f) {
        // Positive prediction error (better than expected)
        float lrp = 1.0f - std::exp(-error * 2.0f);  // Learning rate parameter
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * lrp * 0.8f);
    } else {
        // Negative prediction error (worse than expected)
        float lrn = 1.0f - std::exp(std::abs(error) * 2.0f);
        float decrease = lrn * 0.6f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease);
    }
}

} // namespace nlm
