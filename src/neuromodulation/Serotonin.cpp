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
    float releaseRate;
    float mood;
    float impulsivity;
    float rewardSensitivity;
    float socialAffiliation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.0f)
        , mood(0.5f)
        , impulsivity(0.5f)
        , rewardSensitivity(0.5f)
        , socialAffiliation(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

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
    return 0.4f + 0.6f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialFeedback(float socialReward, float isolation) {
    if (socialReward > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + socialReward * pImpl->releaseRate);
    }
    if (isolation > 0.0f) {
        pImpl->level = std::max(0.0f, pImpl->level - isolation * pImpl->releaseRate * 0.5f);
    }
}

void Serotonin::setMood(float mood) {
    pImpl->mood = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f);
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::modulateDecisionMaking(float decisionValue) {
    if (!pImpl->brain) return;
    
    float modulation = pImpl->level * pImpl->rewardSensitivity;
    
    if (decisionValue > 0.5f) {
        if (pImpl->mood > 0.5f) {
            pImpl->brain->injectCurrentToNeurons(NeuronType::Motor, modulation * 0.2f);
        }
    }
}

void Serotonin::enhanceSocialBonding() {
    if (!pImpl->brain) return;
    
    float bondingEffect = pImpl->level * pImpl->socialAffiliation * 0.3f;
}

} // namespace nlm
