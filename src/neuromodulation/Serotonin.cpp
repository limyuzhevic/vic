#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Social behavior
    float socialRewardValue;
    float socialPunishmentValue;
    
    // Impulsivity control
    float impulsivityLevel;
    float mood;
    
    // Memory effects
    float memoryConsolidationFactor;
    float retrievalEnhancement;
    
    Impl() 
        : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , socialRewardValue(0.0f), socialPunishmentValue(0.0f)
        , impulsivityLevel(0.5f), mood(0.5f)
        , memoryConsolidationFactor(1.0f), retrievalEnhancement(1.0f) {}
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
    
    // Update dependent factors
    pImpl->memoryConsolidationFactor = 0.5f + 0.5f * pImpl->level;
    pImpl->retrievalEnhancement = 0.8f + 0.2f * pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin increases stability and decreases impulsivity-driven plasticity
    float impulsivityEffect = 1.0f - std::abs(pImpl->impulsivityLevel - 0.5f) * 0.5f;
    return 0.8f + 0.2f * pImpl->level * impulsivityEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialReward(float socialValue) {
    pImpl->socialRewardValue = socialValue;
    // Increase serotonin level for positive social experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialValue * pImpl->releaseRate);
}

void Serotonin::signalSocialPunishment(float socialCost) {
    pImpl->socialPunishmentValue = socialCost;
    // Decrease serotonin level for negative social experiences
    pImpl->level = std::max(pImpl->baseline, pImpl->level - socialCost * pImpl->releaseRate * 0.5f);
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

void Serotonin::setImpulsivityLevel(float level) {
    pImpl->impulsivityLevel = std::clamp(level, 0.0f, 1.0f);
}

} // namespace nlm
