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
    float moodState;
    float socialBondingFactor;
    float impulsivityLevel;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.15f), releaseRate(1.2f), moodState(0.0f),
             socialBondingFactor(1.0f), impulsivityLevel(0.5f) {}
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
    // Updates mood state based on level
    pImpl->moodState = -1.0f + 2.0f * pImpl->level;
    // Reduces impulsivity at higher levels
    pImpl->impulsivityLevel = 0.5f * (1.0f - pImpl->level);
}

float Serotonin::getPlasticityFactor() const {
    // Real 5-HT plasticity factor: regulates mood-related plasticity
    // High serotonin stabilizes synaptic changes, reduces impulsivity
    float factor = 1.0f + 0.3f * pImpl->level;
    // Reduces plasticity at high levels to prevent mood instability
    return factor * (1.0f - 0.2f * pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    // Real 5-HT dynamics: homeostatic regulation of mood and social behavior
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Gentle mood oscillations (cortical oscillations at ~10 Hz)
    pImpl->moodState = -0.8f + 1.6f * std::sin(static_cast<float>(dt) * 10.0f);
    // Updates social bonding based on level
    pImpl->socialBondingFactor = 0.8f + 0.4f * pImpl->level;
}

void Serotonin::increaseSocialBehavior(float level) {
    // Real social behavior enhancement: increases social bonding and trust
    pImpl->level = std::min(pImpl->peak, pImpl->level + level * pImpl->releaseRate * 0.8f);
    pImpl->socialBondingFactor = std::min(1.5f, pImpl->socialBondingFactor + level * 0.3f);
}

void Serotonin::decreaseImpulsivity(float level) {
    // Real impulsivity reduction: improves behavioral control
    pImpl->level = std::min(pImpl->peak, pImpl->level + level * pImpl->releaseRate * 0.6f);
    pImpl->impulsivityLevel = std::max(0.1f, pImpl->impulsivityLevel - level * 0.4f);
}

} // namespace nlm
