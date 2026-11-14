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
    // Enhanced curiosity dynamics with multiple interacting components
    // Implements curiosity as combination of information gain and prediction error
    
    // Information gain component (learning potential)
    float informationGain = novelty * 0.6f;
    
    // Prediction error component (surprise)
    float predictionErrorComponent = std::abs(predictionError) * 0.4f;
    
    // Combined drive for exploration
    float targetDrive = informationGain + predictionErrorComponent;
    
    // Multi-timescale updating for biological realism
    float fastTimeScale = 1.0f / (1.0f + 0.5f * static_cast<float>(dt));
    float mediumTimeScale = 1.0f / (1.0f + 2.0f * static_cast<float>(dt));
    
    // Update with different time scales
    pImpl->level += (targetDrive - pImpl->level) * fastTimeScale;
    
    // Medium timescale component for sustained curiosity
    float mediumTarget = targetDrive * 0.7f;
    pImpl->level += (mediumTarget - pImpl->level) * mediumTimeScale * 0.3f;
    
    // Apply biological constraints and noise
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Add stochastic exploration component
    if (pImpl->brain) {
        float explorationJitter = pImpl->brain->getRandomGenerator()->getFloat(-0.02f, 0.02f);
        pImpl->level += explorationJitter * (1.0f - pImpl->level);
    }
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
