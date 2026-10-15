#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float learningRate;
    float maxLevel;
    float baseline;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.6f)
        , predictionErrorWeight(0.4f)
        , decayRate(0.05f)
        , learningRate(0.1f)
        , maxLevel(1.0f)
        , baseline(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized with brain reference");
}

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->maxLevel);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity enhances learning and plasticity for novel experiences
    float effectiveLevel = std::min(pImpl->level / pImpl->maxLevel, 1.0f);
    
    // Cur curiosity drives exploration, which facilitates learning
    float plasticityFactor = 1.0f + pImpl->learningRate * effectiveLevel;
    
    // Higher curiosity increases plasticity but with diminishing returns
    plasticityFactor = std::min(plasticityFactor, 2.0f);
    
    return plasticityFactor;
}

void Curiosity::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * dtFloat);
}

void Curiosity::updateFromInputs(float novelty, float predictionError, TimestepDuration dt) {
    // Update curiosity based on novelty and prediction error signals
    float dtFloat = static_cast<float>(dt);
    
    // Compute target curiosity level from inputs
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    target = std::min(1.0f, target);
    
    // Smooth update towards target
    pImpl->level += (target - pImpl->level) * pImpl->learningRate;
    
    // Decay
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * dtFloat);
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // For backward compatibility - delegate to new method
    updateFromInputs(novelty, predictionError, dt);
}

float Curiosity::getExplorationDrive() const {
    return getLevel();
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

float Curiosity::getNoveltyWeight() const {
    return pImpl->noveltyWeight;
}

float Curiosity::getPredictionErrorWeight() const {
    return pImpl->predictionErrorWeight;
}

void Curiosity::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm