// Curiosity implementation
#include "Curiosity.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float baseline;
    
    Impl() : level(0.0f), noveltyWeight(0.5f), predictionErrorWeight(0.5f), baseline(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(class Brain* brain) {
    // Store brain reference if needed
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity is weighted combination of novelty and prediction error
    float newLevel = pImpl->noveltyWeight * novelty + 
                    pImpl->predictionErrorWeight * predictionError;
    pImpl->level = (pImpl->level * 0.9f) + (newLevel * 0.1f);  // Smooth update
}

float Curiosity::getExplorationDrive() const {
    return getLevel();
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::max(0.0f, std::min(1.0f, weight));
    // Adjust prediction error weight to maintain sum = 1
    pImpl->predictionErrorWeight = 1.0f - pImpl->noveltyWeight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::max(0.0f, std::min(1.0f, weight));
    // Adjust novelty weight to maintain sum = 1
    pImpl->noveltyWeight = 1.0f - pImpl->predictionErrorWeight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
