#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodGain;
    float socialFactor;
    float impulsivityFactor;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.04f)
        , releaseRate(1.2f)
        , moodGain(0.7f)
        , socialFactor(0.5f)
        , impulsivityFactor(0.6f) {}
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
    // Serotonin regulates mood, impulsivity, and social behavior
    // Higher 5-HT generally stabilizes plasticity and reduces impulsivity
    return 0.8f - 0.5f * pImpl->level; // High serotonin reduces high plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodImprovement(float improvement) {
    // 5-HT released during positive experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + improvement * pImpl->releaseRate);
}

void Serotonin::signalSocialBonding() {
    // 5-HT increases during social interactions and bonding
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.4f * pImpl->socialFactor);
}

void Serotonin::signalReducedImpulsivity() {
    // 5-HT reduces impulsive actions
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->impulsivityFactor);
}

} // namespace nlm