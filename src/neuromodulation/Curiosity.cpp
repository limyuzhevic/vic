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

void Curiosity::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    // Curosity modulates plasticity for exploration
    float curiosityFactor = getLevel();
    
    if (stdp) {
        // Higher curiosity increases plasticity for learning new behaviors
        float currentLTP = stdp->getLTPWeight();
        float currentLTD = stdp->getLTDWeight();
        
        // Make learning more flexible when curious
        stdp->setLTPWeight(currentLTP * (1.0f + curiosityFactor * 0.5f));
        stdp->setLTDWeight(currentLTD * (1.0f - curiosityFactor * 0.3f));
    }
    
    if (hebbian) {
        // Hebbian plasticity also modulated by curiosity
        // This allows stronger association of novel experiences
    }
}

} // namespace nlm
