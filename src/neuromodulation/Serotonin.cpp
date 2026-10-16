#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodModulation;
    float impulsivityGain;
    float socialBonding;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             moodModulation(1.0f), impulsivityGain(1.2f), socialBonding(1.0f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior
    // Higher 5-HT decreases impulsivity and increases social bonding
    float socialFactor = 1.0f + pImpl->level * pImpl->socialBonding;
    float impulsivityFactor = 1.0f / (1.0f + pImpl->level * pImpl->impulsivityGain);
    
    return std::clamp(socialFactor * impulsivityFactor, 0.5f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin decays towards baseline, increases with positive social interactions
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalPositiveSocial(float socialReward) {
    // Serotonin increases with positive social interactions
    if (socialReward > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (socialReward - 0.5f) * pImpl->releaseRate * 1.3f);
    }
}

void Serotonin::reduceImpulsivity(float currentImpulsivity) {
    // Serotonin reduces impulsive behavior
    float impulsivityReduction = pImpl->level * 0.3f;
    // This would be applied to action selection system
}

void Serotonin::enhanceSocialLearning(const std::vector<float>& socialPattern) {
    // Serotonin enhances social learning and imitation
    if (!socialPattern.empty()) {
        float patternStrength = 0.0f;
        for (float val : socialPattern) {
            patternStrength += std::abs(val);
        }
        
        float socialLearningBoost = patternStrength * 0.1f * pImpl->level;
        // This would be applied to social learning system
    }
}

} // namespace nlm