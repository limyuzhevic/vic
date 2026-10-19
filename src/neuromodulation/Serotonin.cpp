#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodBoost;
    float impulsivityModulation;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.08f),
             moodBoost(0.4f), impulsivityModulation(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Real implementation: 5-HT affects mood and impulsivity
    return 1.0f + pImpl->level * pImpl->moodBoost;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline with slower decay
    float targetLevel = (pImpl->baseline > 0.0f) ? pImpl->baseline : 0.05f;
    pImpl->level = std::max(targetLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

} // namespace nlm
