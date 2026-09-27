#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float rewardSensitivity;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(1.0f)
        , impulsivity(1.0f)
        , socialBehavior(1.0f)
        , rewardSensitivity(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and behavioral flexibility
    // Higher 5-HT reduces impulsivity and stabilizes behavior
    float moodFactor = 1.0f + pImpl->level * pImpl->mood;
    float stabilityFactor = 1.0f - pImpl->level * 0.3f;  // Reduces impulsivity
    return (moodFactor * stabilityFactor) / 2.0f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalPositiveMood(float intensity) {
    // Signal positive mood (social success, reward)
    pImpl->level = std::min(1.0f, pImpl->level + intensity * pImpl->releaseRate);
    NLM_LOG_INFO("Serotonin positive mood: " + std::to_string(intensity));
}

void Serotonin::signalSocialReward(float value) {
    // Signal social reward (social connection, acceptance)
    pImpl->level = std::min(1.0f, pImpl->level + value * 0.8f * pImpl->releaseRate);
    NLM_LOG_INFO("Serotonin social reward: " + std::to_string(value));
}

void Serotonin::signalStress(float magnitude) {
    // Signal stress (social rejection, threat)
    pImpl->level = std::min(1.0f, pImpl->level + magnitude * 0.6f * pImpl->releaseRate);
    NLM_LOG_INFO("Serotonin stress response: " + std::to_string(magnitude));
}

void Serotonin::setImpulsivityReduction(float reduction) {
    // Reduce impulsivity (behavioral inhibition)
    pImpl->impulsivity = std::max(0.1f, 1.0f - reduction);
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm