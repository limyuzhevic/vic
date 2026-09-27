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
    float moodSensitivity;
    float rewardResponse;
    float punishmentResponse;
    float socialBehavior;
    std::vector<float> moodHistory;
    std::vector<float> socialHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.04f)
        , releaseRate(1.2f)
        , moodSensitivity(0.7f)
        , rewardResponse(0.6f)
        , punishmentResponse(0.4f)
        , socialBehavior(0.8f) {}
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
    // Serotonin modulates plasticity for emotional learning and social behavior
    // Enhances LTP for positive experiences, LTD for negative experiences
    float moodBasedFactor = 0.5f + 0.5f * (pImpl->level - 0.5f); // Center around 0.5
    return std::max(0.1f, std::min(1.0f, moodBasedFactor));
}

void Serotonin::update(TimestepDuration dt) {
    // Tonic baseline with gradual decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalReward(float reward) {
    // Increase serotonin in response to rewards
    if (reward > 0.0f) {
        float response = reward * pImpl->rewardResponse;
        pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate);
        pImpl->moodHistory.push_back(pImpl->level);
    }
}

void Serotonin::signalPunishment(float punishment) {
    // Decrease serotonin in response to punishments (phasic response)
    if (punishment > 0.0f) {
        float response = punishment * pImpl->punishmentResponse;
        pImpl->level = std::max(0.0f, pImpl->level - response * pImpl->releaseRate);
    }
}

void Serotonin::modulateSocialBehavior(float socialStrength) {
    // Enhance social behavior modulation
    if (socialStrength > 0.0f) {
        float enhancement = socialStrength * pImpl->socialBehavior;
        pImpl->level = std::min(pImpl->peak, pImpl->level + enhancement * pImpl->releaseRate);
        pImpl->socialHistory.push_back(pImpl->level);
    }
}

} // namespace nlm