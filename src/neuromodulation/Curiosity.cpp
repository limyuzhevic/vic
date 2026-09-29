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
    // Enhanced curiosity dynamics with temporal integration
    // Combines novelty detection with prediction error for optimal exploration
    
    float dtf = static_cast<float>(dt);
    
    // Compute weighted novelty and prediction error
    float noveltyComponent = pImpl->noveltyWeight * novelty;
    float predictionErrorComponent = pImpl->predictionErrorWeight * predictionError;
    
    // Apply threshold: only respond to meaningful signals
    const float noveltyThreshold = 0.1f;
    const float predictionErrorThreshold = 0.05f;
    
    if (std::abs(novelty) < noveltyThreshold && std::abs(predictionError) < predictionErrorThreshold) {
        // No significant signal, decay naturally
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dtf);
        return;
    }
    
    // Compute target curiosity level
    float target = noveltyComponent + predictionErrorComponent;
    
    // Apply nonlinear transformation for more realistic behavior
    // Curiosity increases sublinearly with input to prevent runaway excitation
    target = target * target / (target + 0.1f);
    
    // Temporal integration with adaptive learning rate
    float learningRate = 0.15f;  // Base learning rate
    
    // Speed up learning when signals are strong
    if (std::abs(target) > 0.5f) {
        learningRate = 0.25f;
    }
    
    // Smooth integration using exponential moving average
    pImpl->level += (target - pImpl->level) * learningRate * dtf;
    
    // Apply biological constraints: curiosity level between 0 and 1
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Natural decay when not stimulated
    if (target < 0.01f) {
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dtf);
    }
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
    pImpl->brain = nullptr;
}

} // namespace nlm
