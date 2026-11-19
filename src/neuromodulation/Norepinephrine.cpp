#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalBoost;
    float workingMemoryBoost;
    float vigilanceIncrease;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f),
             arousalBoost(1.5f), workingMemoryBoost(1.3f), vigilanceIncrease(1.2f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances arousal and working memory capacity
    return 0.9f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseAlertness(float strength) {
    // Boost arousal and alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * arousalBoost);
}

void Norepinephrine::enhanceWorkingMemory(float factor) {
    // Enhance working memory capacity and maintenance
    pImpl->workingMemoryBoost = std::max(1.0f, pImpl->workingMemoryBoost + factor * 0.4f);
}

} // namespace nlm
