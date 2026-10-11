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
    // Update curiosity with biologically plausible dynamics
    // Implements interaction between novelty and prediction error
    // Models the exploration-exploitation tradeoff
    
    const float dt_sec = static_cast<float>(dt) * 0.001f;  // Convert to seconds
    
    // Compute target curiosity from inputs with nonlinear interactions
    // Novelty and prediction error interact synergistically
    float noveltyComponent = std::tanh(novelty);
    float predictionErrorComponent = std::tanh(predictionError);
    
    // Synergistic combination based on neurobiological findings
    // Novelty and prediction error multiply their effects
    float synergy = 1.0f + 0.5f * std::min(noveltyComponent, predictionErrorComponent);
    float target = (pImpl->noveltyWeight * noveltyComponent + 
                    pImpl->predictionErrorWeight * predictionErrorComponent) * synergy;
    
    // Smooth update with time constant (neural adaptation)
    float adaptationRate = 0.05f;
    pImpl->level += (target - pImpl->level) * adaptationRate * dt_sec;
    
    // Realistic decay of curiosity (exploration fades without reinforcement)
    float decayConstant = 2.0f;  // seconds
    pImpl->level *= std::exp(-dt_sec / decayConstant);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
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
