#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float workingMemoryGain;
    float predictionErrorIntegration;
    float temporalHalfLife;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , workingMemoryGain(0.5f)
        , predictionErrorIntegration(0.7f)
        , temporalHalfLife(10.0f) {}
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
    // Enhanced dopamine-modulated plasticity with prediction error integration
    float peModulation = 1.0f + pImpl->predictionErrorIntegration * 0.5f;
    return 0.5f + 0.5f * pImpl->level * peModulation;
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Apply temporal dynamics (exponential decay)
    float temporalDecay = std::exp(-timeStep / pImpl->temporalHalfLife);
    pImpl->level *= temporalDecay;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
}

void Dopamine::signalReward(float reward) {
    float releaseAmount = reward * pImpl->releaseRate;
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    // Enhance working memory when dopamine increases
    if (pImpl->brain && getLevel() > 0.5f) {
        pImpl->brain->getWorkingMemory()->setDopamineLevel(getLevel() * pImpl->workingMemoryGain);
    }
    
    NLM_LOG_INFO("Dopamine released: " + std::to_string(getLevel()) + " (reward: " + std::to_string(reward) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced prediction error signaling with working memory integration
    float releaseAmount = std::abs(error) * pImpl->releaseRate * pImpl->predictionErrorIntegration;
    setLevel(std::min(pImpl->peak, getLevel() + releaseAmount));
    
    // Update working memory with prediction error information
    if (pImpl->brain && getLevel() > 0.3f) {
        float peModulation = getLevel() * 0.3f;
        pImpl->brain->getWorkingMemory()->setPredictionErrorModulation(peModulation);
    }
    
    NLM_LOG_INFO("Dopamine prediction error: " + std::to_string(getLevel()) + " (error: " + std::to_string(error) + ")");
}

} // namespace nlm