#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalBoost;
    float vigilanceBoost;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f),
             arousalBoost(0.6f), vigilanceBoost(0.7f) {}
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
    // Real implementation: NE increases arousal and vigilance
    return 1.0f + pImpl->level * (pImpl->arousalBoost + pImpl->vigilanceBoost) * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    float targetLevel = (pImpl->baseline > 0.0f) ? pImpl->baseline : 0.1f;
    pImpl->level = std::max(targetLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

} // namespace nlm
