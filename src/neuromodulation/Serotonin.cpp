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
    
    // Mood and affect state
    float mood;
    float rewardSensitivity;
    float impulsivity;
    
    // Social behavior modulation
    float socialBonding;
    float socialReward;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , mood(0.5f)
        , rewardSensitivity(0.5f)
        , impulsivity(0.5f)
        , socialBonding(0.5f)
        , socialReward(0.5f) {}
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
    // Serotonin modulates plasticity in complex ways
    // Can enhance or suppress depending on context
    float plasticity = 1.0f;
    
    if (pImpl->level > 0.5f) {
        // High serotonin typically reduces impulsivity and can enhance certain types of learning
        plasticity = 0.8f;
    } else if (pImpl->level < 0.3f) {
        // Low serotonin often associated with increased impulsivity
        plasticity = 1.3f;
    }
    
    return plasticity;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay mood and affect states
    pImpl->mood = std::max(0.1f, pImpl->mood - 0.01f * static_cast<float>(dt));
    pImpl->rewardSensitivity = std::max(0.1f, pImpl->rewardSensitivity - 0.01f * static_cast<float>(dt));
    pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity - debounce(0.01f * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodChange) {
    // Change in emotional state
    pImpl->mood = std::min(1.0f, std::max(0.1f, pImpl->mood + moodChange * 0.2f));
    
    // Modulate serotonin level based on mood changes
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodChange * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalReward(float reward) {
    // Reward processing increases serotonin
    pImpl->rewardSensitivity = std::min(1.0f, pImpl->rewardSensitivity + reward * 0.1f);
    
    // Social reward processing
    pImpl->socialReward = std::min(1.0f, pImpl->socialReward + reward * 0.1f);
    
    // Modulate serotonin level based on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.3f);
}

void Serotonin::signalSocialBonding(float bondingStrength) {
    // Social connection processing
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + bondingStrength * 0.2f);
    
    // Social serotonin release
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondingStrength * pImpl->releaseRate * 0.8f);
}

void Serotonin::setMood(float moodLevel) {
    pImpl->mood = std::clamp(moodLevel, 0.1f, 1.0f);
}

void Serotonin::setRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = std::clamp(sensitivity, 0.1f, 1.0f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

float Serotonin::getSocialBonding() const {
    return pImpl->socialBonding;
}

} // namespace nlm