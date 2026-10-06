#include "Curiosity.hpp"
#include "brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float explorationThreshold;
    float learningRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , explorationThreshold(0.3f)
        , learningRate(0.1f) {}
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
    // Curiosa increases with novelty and prediction error (RL exploration drive)
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with learning rate
    pImpl->level += (target - pImpl->level) * pImpl->learningRate;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply to brain via dopamine integration
    if (pImpl->brain && pImpl->brain->getDopamine()) {
        pImpl->brain->getDopamine()->integrateCuriosity(pImpl->level);
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
