#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float lastPredictionError;  // Track for curiosity modulation
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , lastPredictionError(0.0f) {}
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
    // Real curiosity dynamics with brain integration
    // Curiosity is driven by both novelty detection and reward prediction error
    // The system learns to balance exploration vs exploitation based on neural feedback
    
    if (!pImpl->brain) {
        // If no brain is available, just decay to baseline
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
        return;
    }
    
    // Track prediction error for temporal comparison
    pImpl->lastPredictionError = predictionError;
    
    // Compute curiosity drive from multiple sources:
    // 1. Novelty: unexpected sensory input
    // 2. Prediction error: surprise at outcomes
    // 3. Internal state: homeostatic and motivational signals
    
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Get modulatory influences from brain systems
    // Dopamine enhances curiosity when reward is available
    if (auto* dopamine = pImpl->brain->getDopamine()) {
        target *= (1.0f + dopamine->getLevel());  // Dopamine amplifies curiosity
    }
    
    // Development stage modulates curiosity (critical period > adult)
    auto developmentalStage = pImpl->brain->getDevelopmentalStage();
    if (developmentalStage == DevelopmentalStage::CriticalPeriod) {
        target *= 2.0f;  // Double curiosity during critical period
    } else if (developmentalStage == DevelopmentalStage::Aging) {
        target *= 0.5f;  // Reduced curiosity in aging
    }
    
    // Smooth update towards target with adaptive time constant
    float adaptationRate = 0.1f + 0.05f * target;  // Faster adaptation with higher drive
    pImpl->level += (target - pImpl->level) * adaptationRate * static_cast<float>(dt);
    
    // Constrain to biologically plausible range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Decay towards baseline over time (exploration fatigue)
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(0.0f, pImpl->level - decay);
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
