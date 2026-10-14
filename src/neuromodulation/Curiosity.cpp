#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float arousalWeight;
    float stressWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.4f)
        , predictionErrorWeight(0.3f)
        , arousalWeight(0.2f)
        , stressWeight(0.1f)
        , decayRate(0.05f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty, prediction error, arousal, and stress
    // Weighted integration of multiple signals
    float norepinephrineLevel = 0.5f; // Would get from brain if NE is available
    if (pImpl->brain && pImpl->brain->getNorepinephrine()) {
        norepinephrineLevel = pImpl->brain->getNorepinephrine()->getLevel();
    }
    
    float stressLevel = 0.3f; // Would get from brain if 5-HT is available
    if (pImpl->brain && pImpl->brain->getSerotonin()) {
        stressLevel = 1.0f - pImpl->brain->getSerotonin()->getLevel(); // Inverted: high stress = low 5-HT
    }
    
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError +
                   pImpl->arousalWeight * norepinephrineLevel +
                   pImpl->stressWeight * stressLevel;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Curiosity::update(float novelty, float predictionError, float arousal, TimestepDuration dt) {
    // Enhanced update with arousal parameter for NE integration
    float stressLevel = 0.3f; // Default stress
    if (pImpl->brain && pImpl->brain->getSerotonin()) {
        stressLevel = 1.0f - pImpl->brain->getSerotonin()->getLevel();
    }
    
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError +
                   pImpl->arousalWeight * arousal +
                   pImpl->stressWeight * stressLevel;
    
    pImpl->level += (target - pImpl->level) * 0.1f;
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::setArousalWeight(float weight) {
    pImpl->arousalWeight = weight;
}

void Curiosity::setStressWeight(float weight) {
    pImpl->stressWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
