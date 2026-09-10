#include "Curiosity.hpp"
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
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
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

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

float Curiosity::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real curiosity-modulated plasticity factor
    // PLACEHOLDER: Higher curiosity increases exploration-based plasticity
    return 0.5f + 0.5f * pImpl->level;
}
