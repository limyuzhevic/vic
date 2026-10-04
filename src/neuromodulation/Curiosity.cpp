#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float explorationThreshold;
    float maxExploration;
    float learningRate;
    float decayRate;
    float attentionModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.6f)
        , predictionErrorWeight(0.4f)
        , explorationThreshold(0.3f)
        , maxExploration(1.0f)
        , learningRate(0.15f)
        , decayRate(0.08f)
        , attentionModulation(1.0f) {}
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
    // Curiosity is driven by novelty and prediction error (exploration motivation)
    // This implements an intrinsic reward signal for exploration
    
    // Compute current curiosity drive components
    float noveltyComponent = pImpl->noveltyWeight * novelty;
    float predictionErrorComponent = pImpl->predictionErrorWeight * std::abs(predictionError);
    
    // Combine with attention modulation from the brain
    if (pImpl->brain && pImpl->attentionModulation > 0.0f) {
        // Attention system modulates curiosity
        // Higher attention increases curiosity drive
        noveltyComponent *= pImpl->attentionModulation;
        predictionErrorComponent *= pImpl->attentionModulation;
    }
    
    // Target curiosity level based on intrinsic motivation
    float target = std::min(noveltyComponent + predictionErrorComponent, pImpl->maxExploration);
    
    // Smooth update using adaptive learning rate
    float learningRate = pImpl->learningRate;
    if (std::abs(target - pImpl->level) < 0.01f) {
        learningRate *= 2.0f;  // Faster convergence when close to target
    }
    
    pImpl->level += (target - pImpl->level) * learningRate;
    
    // Natural decay over time (conservation of energy principle)
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Ensure within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->maxExploration);
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setExplorationThreshold(float threshold) {
    pImpl->explorationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Curiosity::setAttentionModulation(float modulation) {
    pImpl->attentionModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
