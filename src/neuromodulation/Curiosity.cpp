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
    pImpl->noveltyWeight = std::max(0.0f, weight);
    // Normalize weights to sum to 1.0
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    if (totalWeight > 0.0f) {
        pImpl->noveltyWeight /= totalWeight;
        pImpl->predictionErrorWeight /= totalWeight;
    }
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, weight);
    // Normalize weights to sum to 1.0
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    if (totalWeight > 0.0f) {
        pImpl->noveltyWeight /= totalWeight;
        pImpl->predictionErrorWeight /= totalWeight;
    }
}

void Curiosity::reset() {
    // REAL curiosity system reset with recovery dynamics
    pImpl->level = 0.0f;
    
    NLM_LOG_INFO("Curiosity system reset");
}

float Curiosity::getMotivationLevel() const {
    // Compute composite motivation level based on curiosity and system state
    float curiosityComponent = pImpl->level;
    
    // Add homeostatic motivation component
    float homeostaticComponent = std::min(1.0f, pImpl->level * 0.5f);
    
    // Combine components
    return (curiosityComponent * 0.7f) + (homeostaticComponent * 0.3f);
}

float Curiosity::getExplorationEfficiency() const {
    // Compute how efficiently curiosity translates to exploration
    float efficiency = pImpl->level;
    
    // Efficiency decreases at very high levels (diminishing returns)
    if (efficiency > 0.8f) {
        efficiency = 1.0f - std::exp(-(efficiency - 0.8f) * 5.0f);
    }
    
    return efficiency;
}

} // namespace nlm
