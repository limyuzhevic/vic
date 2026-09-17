#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodBoost;
    float impulsivityMod;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f), releaseRate(1.0f),
             moodBoost(1.0f), impulsivityMod(1.0f) {}
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
    // 5-HT modulates mood and impulsivity
    // Affects decision making and social behavior
    float moodMod = 1.0f + pImpl->level * 0.2f - 0.5f;  // Can be inhibitory at high levels
    
    return std::clamp(moodMod, 0.3f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slower decay
    pImpl->moodBoost = pImpl->moodBoost * 0.95f + 1.0f * 0.05f;
    pImpl->impulsivityMod = pImpl->impulsivityMod * 1.1f + 1.0f * 0.1f;  // 5-HT reduces impulsivity
}

void Serotonin::signalMoodBoost(float moodLevel) {
    // Signal improved mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate);
}

void Serotonin::signalSocialReward(float socialReward) {
    // Signal social reward/rejection
    pImpl->level = std::clamp(pImpl->level + socialReward * pImpl->releaseRate, 0.0f, pImpl->peak);
}

} // namespace nlm