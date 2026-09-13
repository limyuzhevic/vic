#include "Novelty.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float activationHistory;
    
    Impl() 
        : level(0.0f)
        , baseline(0.01f)
        , peak(1.0f)
        , decayRate(0.1f)
        , activationHistory(0.0f) {}
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
    // Novelty enhances plasticity for exploration
    return 0.5f + 0.5f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::detectNovelty(float activation, float noveltyThreshold) {
    // Detect novel patterns (activation significantly different from baseline)
    if (std::abs(activation) > noveltyThreshold) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(activation - noveltyThreshold) * 0.5f);
    }
}

void Novelty::integrateWithCuriosity(float curiosityLevel) {
    // Novelty and curiosity have complementary effects
    if (pImpl->level > 0.5f && curiosityLevel > 0.5f) {
        // High novelty + high curiosity = strong exploration drive
        setLevel(1.0f);
    }
}

} // namespace nlm
