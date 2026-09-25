#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

struct Curiosity::Impl {
    Brain* brain;
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
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize Curiosity: brain pointer is null");
        return;
    }
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Curiosity update called without brain initialization");
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
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
