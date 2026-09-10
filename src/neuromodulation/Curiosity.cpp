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
    
    ~Impl() {
        brain = nullptr;
        level = 0.0f;
        noveltyWeight = 0.0f;
        predictionErrorWeight = 0.0f;
        decayRate = 0.0f;
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Null pointer provided to Curiosity::initialize");
        return;
    }
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    if (!std::isfinite(novelty) || !std::isfinite(predictionError)) {
        NLM_LOG_WARNING("Invalid novelty or prediction error value");
        novelty = std::max(0.0f, novelty);
        predictionError = std::max(0.0f, predictionError);
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
    if (!std::isfinite(weight) || weight < 0.0f) {
        NLM_LOG_WARNING("Invalid novelty weight, using default 0.5f");
        weight = 0.5f;
    }
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    if (!std::isfinite(weight) || weight < 0.0f) {
        NLM_LOG_WARNING("Invalid prediction error weight, using default 0.5f");
        weight = 0.5f;
    }
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
