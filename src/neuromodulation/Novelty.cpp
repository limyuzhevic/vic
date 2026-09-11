#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float previousVisionHash;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), previousVisionHash(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty affects plasticity by promoting exploration and learning
    return 0.3f + 0.7f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real novelty detection
    // PLACEHOLDER: Decay novelty over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::detectNovelty(float newVisionHash, float newInternalSignals) {
    // TODO PHASE 2: Implement real novelty detection from sensory input
    // PLACEHOLDER: Novelty is difference from previous state
    float visionNovelty = std::abs(newVisionHash - pImpl->previousVisionHash);
    float internalNovelty = std::abs(newInternalSignals - pImpl->level);
    
    float totalNovelty = (visionNovelty + internalNovelty) * 0.5f;
    
    if (totalNovelty > pImpl->level) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + totalNovelty * pImpl->releaseRate);
    }
    
    pImpl->previousVisionHash = newVisionHash;
}

} // namespace nlm