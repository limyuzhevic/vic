// Curiosity.cpp - Implementation
#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    Brain* brain;
    float curiosityLevel;
    float noveltyHistory;
    float predictionErrorHistory;
    float explorationBias;
    
    Impl() : brain(nullptr), curiosityLevel(0.0f), noveltyHistory(0.0f), 
             predictionErrorHistory(0.0f), explorationBias(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

void Curiosity::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Get current novelty and prediction error
    auto* novelty = pImpl->brain->getNovelty();
    auto* predError = pImpl->brain->getPredictionErrorSignal();
    
    float noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
    float errorLevel = predError ? predError->getLevel() : 0.0f;
    
    // Update history
    pImpl->noveltyHistory = pImpl->noveltyHistory * 0.9f + noveltyLevel * 0.1f;
    pImpl->predictionErrorHistory = pImpl->predictionErrorHistory * 0.9f + errorLevel * 0.1f;
    
    // Calculate curiosity as weighted combination
    float noveltyDrive = pImpl->noveltyHistory * 0.6f;
    float uncertaintyDrive = pImpl->predictionErrorHistory * 0.4f;
    
    pImpl->curiosityLevel = noveltyDrive + uncertaintyDrive;
    pImpl->curiosityLevel = std::min(1.0f, pImpl->curiosityLevel);
    
    // Apply curiosity effects
    pImpl->explorationBias = pImpl->curiosityLevel * 0.5f;
}

float Curiosity::getLevel() const {
    return pImpl->curiosityLevel;
}

float Curiosity::getExplorationBias() const {
    return pImpl->explorationBias;
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity enhances plasticity for novel experiences
    return 1.0f + pImpl->curiosityLevel * 0.3f;
}

} // namespace nlm