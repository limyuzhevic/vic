#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float efficacy;
    float mood;
    float impulsivity;
    float socialAttraction;
    float attentionBoost;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.1f), decayRate(0.05f), 
             efficacy(1.0f), mood(0.5f), impulsivity(0.5f), socialAttraction(0.5f), 
             attentionBoost(0.3f) {}
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
    // Serotonin modulates plasticity, generally decreasing it for learning new associations
    // but enhancing consolidation of strong memories
    return 0.8f - 0.3f * pImpl->level;  // Reduced plasticity at higher levels
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin decays with a baseline level
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update related functions based on level
    pImpl->mood = 0.5f + 0.5f * pImpl->level;
    pImpl->impulsivity = 1.0f - 0.5f * pImpl->level;
    pImpl->socialAttraction = 0.3f + 0.4f * pImpl->level;
    pImpl->attentionBoost = 0.2f + 0.6f * pImpl->level;
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds less to small rewards than dopamine
    pImpl->level = std::min(1.0f, pImpl->level + reward * 0.3f);
}

void Serotonin::signalSustainedReward(float reward) {
    // Serotonin integrates over sustained reward to signal satiety/happiness
    pImpl->level = std::min(1.0f, pImpl->level + reward * 0.5f);
}

void Serotonin::setMoodModifier(float modifier) {
    pImpl->mood = std::clamp(modifier, 0.0f, 1.0f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialAttraction() const {
    return pImpl->socialAttraction;
}

float Serotonin::getAttentionBoost() const {
    return pImpl->attentionBoost;
}

void Serotonin::clearHistory() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm