#include "Curiosity.hpp"
#include "NeuromodulatorBase.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {}
};

Curiosity::Curiosity() : NeuromodulatorBase(), pImpl(new Impl) {
    currentLevel = 0.0f;
    lastUpdateTime = 0;
    strength = 1.0f;
}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return currentLevel;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    currentLevel = pImpl->level;
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update using common helper
    applySmoothing(currentLevel, target, 0.1f);
    
    // Decay using common helper
    currentLevel = computeDecay(currentLevel, pImpl->decayRate, dt);
}

float Curiosity::getExplorationDrive() const {
    return currentLevel;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    currentLevel = 0.0f;
}

} // namespace nlm
