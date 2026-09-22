#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float baseline;
    float decayRate;
    float predictionErrorAmplifier;
    float noveltyThreshold;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.05f), 
             predictionErrorAmplifier(2.0f), noveltyThreshold(0.3f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity enhances learning and exploration
    if (pImpl->level < pImpl->noveltyThreshold) {
        return 1.0f + pImpl->level * 0.3f;
    } else {
        // High curiosity = very high plasticity for exploration
        return 1.5f + (pImpl->level - pImpl->noveltyThreshold) * 0.5f;
    }
}

void Curiosity::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Curiosity::updateWithNovelty(float novelty) {
    // Increase curiosity based on novelty
    if (novelty > 0.0f) {
        pImpl->level = std::min(1.0f, pImpl->level + novelty * pImpl->predictionErrorAmplifier * 0.1f);
    }
}

void Curiosity::updateWithPredictionError(float predictionError) {
    // Increase curiosity based on prediction error
    if (predictionError != 0.0f) {
        float errorMagnitude = std::abs(predictionError);
        pImpl->level = std::min(1.0f, pImpl->level + errorMagnitude * pImpl->predictionErrorAmplifier * 0.05f);
    }
}

float Curiosity::getPredictionErrorAmplifier() const {
    return pImpl->predictionErrorAmplifier;
}

} // namespace nlm