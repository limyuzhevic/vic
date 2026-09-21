#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulsivity;
    float socialBehavior;
    float inhibitionStrength;
    std::vector<float> rewardHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.12f)
        , moodLevel(1.0f)
        , impulsivity(0.5f)
        , socialBehavior(0.7f)
        , inhibitionStrength(1.0f) {}
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
    // Serotonin regulates mood, impulsivity, and behavioral inhibition
    float moodBoost = 1.0f + pImpl->moodLevel * pImpl->level * 0.3f;
    float inhibitionBoost = 1.0f + pImpl->inhibitionStrength * pImpl->level * 0.8f;
    return moodBoost * inhibitionBoost;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalRewardSatisfaction(float rewardHistory, float mood) {
    // Serotonin responds to reward history and mood state
    if (rewardHistory > 0.0f) {
        float boost = std::min(1.0f, rewardHistory * mood * 0.5f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * 0.4f);
    }
}

void Serotonin::regulateImpulsivity(NeuronId neuronId, float currentDrive, float goalRelevance) {
    // Modulate impulsive behavior and behavioral inhibition
    if (pImpl->level > 0.1f) {
        // Would enhance inhibitory control in specific circuits
        float regulationEffect = (1.0f - currentDrive) * goalRelevance * pImpl->level;
        if (pImpl->brain) {
            NLM_LOG_INFO("5-HT regulating impulsivity with effect " + std::to_string(regulationEffect));
        }
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm