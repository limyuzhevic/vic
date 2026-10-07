#include "Curiosity.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float explorationDrive;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float acquisitionRate;
    
    Impl() : level(0.0f), explorationDrive(0.0f), noveltyWeight(0.5f), 
             predictionErrorWeight(0.5f), decayRate(0.1f), acquisitionRate(1.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(class Brain* brain) {
    // Store brain reference if needed for future integration
    // Currently placeholder implementation
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // ========== CURIOUSY UPDATE LOGIC ==========
    // Curiosity is driven by both novelty and prediction error
    // This implements a weighted combination of both signals
    
    // Calculate curiosity as weighted sum of novelty and prediction error
    float curiositySignal = pImpl->noveltyWeight * novelty + 
                           pImpl->predictionErrorWeight * predictionError;
    
    // Apply acquisition rate to limit growth speed
    float curiosityGain = 1.0f - std::exp(-pImpl->acquisitionRate * dt);
    pImpl->level += (curiositySignal - pImpl->level) * curiosityGain;
    
    // Decay curiosity over time to prevent runaway excitation
    float decayFactor = std::exp(-pImpl->decayRate * dt);
    pImpl->level *= decayFactor;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Exploration drive is proportional to curiosity level
    pImpl->explorationDrive = pImpl->level;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
    // Normalize weights to maintain total weight of 1.0
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    if (totalWeight > 0.0f) {
        pImpl->noveltyWeight /= totalWeight;
        pImpl->predictionErrorWeight /= totalWeight;
    }
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
    // Normalize weights to maintain total weight of 1.0
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    if (totalWeight > 0.0f) {
        pImpl->noveltyWeight /= totalWeight;
        pImpl->predictionErrorWeight /= totalWeight;
    }
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationDrive = 0.0f;
}

} // namespace nlm