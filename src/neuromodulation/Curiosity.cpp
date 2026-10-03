#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float noveltyWeight;
    float predictionErrorWeight;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.05f), noveltyWeight(0.5f), predictionErrorWeight(0.5f) {}
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

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity drives exploration and learning
    return 1.0f + pImpl->level * 1.5f;
}

void Curiosity::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Curiosity::updateFromNovelty(float novelty, float predictionError) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * 0.1f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm