#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    std::weak_ptr<class Brain> brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    bool initialized;
    
    Impl() 
        : brain()
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , initialized(false) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() {
    delete pImpl;
}

void Curiosity::initialize(class Brain* brain) {
    if (brain) {
        pImpl->brain = std::shared_ptr<class Brain>(brain, [](class Brain*) {});
        pImpl->initialized = true;
        NLM_LOG_INFO("Curiosity system initialized");
    }
}

bool Curiosity::isInitialized() const {
    return pImpl->initialized && !pImpl->brain.expired();
}

float Curiosity::getLevel() const {
    if (!isInitialized()) {
        return 0.0f;
    }
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    if (!isInitialized()) {
        return;
    }
    
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

} // namespace nlm