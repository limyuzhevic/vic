#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalStrength;
    float vigilanceStrength;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), arousalStrength(1.0f), 
             vigilanceStrength(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {}

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
    // Norepinephrine modulates arousal and enhances attention
    return 1.0f + 0.3f * pImpl->arousalStrength + 0.2f * pImpl->vigilanceStrength;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float strength) {
    pImpl->arousalStrength = std::min(1.0f, pImpl->arousalStrength + strength);
}

void Norepinephrine::enhanceVigilance(float strength) {
    pImpl->vigilanceStrength = std::min(1.0f, pImpl->vigilanceStrength + strength);
}

} // namespace nlm