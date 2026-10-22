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
    // Validate input bounds
    if (novelty < 0.0f || novelty > 10.0f) {  // Reasonable novelty bounds
        NLM_LOG_WARNING("Novelty value out of bounds: " + std::to_string(novelty));
        novelty = std::clamp(novelty, 0.0f, 10.0f);
    }
    
    if (predictionError < 0.0f || predictionError > 5.0f) {  // Reasonable prediction error bounds
        NLM_LOG_WARNING("Prediction error value out of bounds: " + std::to_string(predictionError));
        predictionError = std::clamp(predictionError, 0.0f, 5.0f);
    }
    
    // Ensure timestep is positive
    TimestepDuration validDt = std::max(dt, 0.0);
    if (validDt == 0.0) {
        NLM_LOG_WARNING("Zero timestep in Curiosity update");
        return;
    }
    
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with bounds checking
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Clamp to reasonable range [0.0f, 10.0f]
    pImpl->level = std::clamp(pImpl->level, 0.0f, 10.0f);
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(validDt));
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
