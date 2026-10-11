#include "BasePimpl.hpp"
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
    pImpl->get()->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->get()->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->get()->noveltyWeight * novelty + 
                   pImpl->get()->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->get()->level += (target - pImpl->get()->level) * 0.1f;
    
    // Decay
    pImpl->get()->level = std::max(0.0f, pImpl->get()->level - pImpl->get()->decayRate * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->get()->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->get()->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->get()->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->get()->level = 0.0f;
}

} // namespace nlm
