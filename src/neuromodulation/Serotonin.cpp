#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float mood;
    float impulsivity;
    float socialBehavior;
    float decayRate;
    float rewardHistory;
    std::vector<float> history;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.05f), mood(0.0f), impulsivity(0.0f),
             socialBehavior(0.5f), decayRate(0.03f), rewardHistory(0.0f) {}
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
    // Serotonin affects mood and impulsivity, modulating plasticity
    return 0.8f + 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay with some baseline maintenance
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood and related states
    pImpl->mood = 0.3f + 0.7f * pImpl->level;
    pImpl->impulsivity = 0.5f + 0.5f * pImpl->level;
    pImpl->socialBehavior = 0.5f + 0.5f * pImpl->level;
    
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - mood, impulsivity, social behavior");
}

void Serotonin::signalReward(float reward) {
    // Positive reward increases serotonin level
    pImpl->rewardHistory = reward;
    pImpl->level = std::min(1.0f, pImpl->level + reward * 0.15f);
}

void Serotonin::signalSocialInteraction(float socialValue) {
    // Social interactions increase serotonin
    pImpl->level = std::min(1.0f, pImpl->level + socialValue * 0.2f);
}

void Serotonin::signalPunishment(float punishment) {
    // Negative events decrease serotonin
    pImpl->level = std::max(pImpl->baseline, pImpl->level - punishment * 0.25f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

} // namespace nlm