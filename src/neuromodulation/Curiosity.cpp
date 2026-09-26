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
    // Phase 2: Implement enhanced curiosity dynamics
    // Biological evidence shows curiosity grows with prediction error
    // and decays exponentially, with adaptation to new levels
    
    // Compute target curiosity level based on inputs
    float noveltyContribution = pImpl->noveltyWeight * std::min(1.0f, novelty);
    float predictionErrorContribution = pImpl->predictionErrorWeight * std::min(1.0f, std::abs(predictionError));
    
    float targetLevel = noveltyContribution + predictionErrorContribution;
    
    // Apply differential dynamics based on prediction error sign
    if (predictionError > 0.0f) {
        // Positive prediction error (surprise) increases curiosity
        pImpl->level += (targetLevel - pImpl->level) * 0.3f;  // Faster adaptation
        // Novelty resets decay timer
        pImpl->decayRate = 0.02f;  // Lower decay when actively exploring
    } else if (predictionError < 0.0f) {
        // Negative prediction error (predictable) decreases curiosity
        pImpl->level += (targetLevel - pImpl->level) * 0.15f;  // Slower adaptation
        // Resume normal decay
        pImpl->decayRate = 0.05f;
    } else {
        // Zero prediction error: normal curiosity dynamics
        pImpl->level += (targetLevel - pImpl->level) * 0.1f;
    }
    
    // Apply decay based on current level and decay rate
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply exploration-exploitation tradeoff
    // High curiosity should encourage exploration
    if (pImpl->level > 0.7f) {
        // Exploration mode: high level, slow decay
        pImpl->level = std::min(1.0f, pImpl->level);
        pImpl->decayRate = 0.01f;  // Very slow decay in exploration
    } else if (pImpl->level > 0.3f) {
        // Moderate mode
        pImpl->decayRate = 0.03f;
    }
    
    // Add small stochastic component for behavioral realism
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f * pImpl->level;
    pImpl->level += noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

// Add missing getExplorationDrive method implementation
float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->decayRate = 0.05f;
}

} // namespace nlm
