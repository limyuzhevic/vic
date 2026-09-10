#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float socialBonding;
    float impulsivityControl;
    float decayRate;
    float baseline;
    float peak;
    float rewardHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(0.0f)
        , socialBonding(0.0f)
        , impulsivityControl(0.0f)
        , decayRate(0.08f)
        , baseline(0.0f)
        , peak(1.0f)
        , rewardHistory(0.0f) {}
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin reduces plasticity for stable memory formation
    // High levels promote long-term memory over short-term
    return 1.0f - pImpl->level * 0.4f;
}

void Serotonin::update(TimestepDuration dt, float novelty, 
                      float predictionError, float reward) {
    // Serotonin responds to reward prediction errors and social feedback
    float moodDrive = 0.0f;
    
    if (predictionError != 0.0f) {
        // Positive prediction error increases serotonin (good outcome)
        // Negative prediction error decreases serotonin
        moodDrive += predictionError * 0.5f;
    }
    
    if (reward != 0.0f) {
        // Reward learning modulates serotonin
        moodDrive += reward * 0.3f;
        pImpl->rewardHistory = std::min(pImpl->rewardHistory + reward, 1.0f);
    }
    
    if (novelty != 0.0f) {
        // Novelty can increase serotonin for exploration rewards
        moodDrive += novelty * 0.2f;
    }
    
    // Update mood and social bonding
    pImpl->mood += moodDrive * 0.2f - pImpl->decayRate * static_cast<float>(dt);
    pImpl->mood = std::clamp(pImpl->mood, -0.5f, 1.0f);
    
    // Social bonding from positive experiences
    if (pImpl->mood > 0.2f) {
        pImpl->socialBonding = std::min(pImpl->socialBonding + 0.1f, 1.0f);
    }
    
    // Impulse control improves with mood stability
    pImpl->impulsivityControl = 0.5f + pImpl->mood * 0.5f;
    
    // Update serotonin level based on mood
    pImpl->level = std::clamp(pImpl->mood + 0.5f, pImpl->baseline, pImpl->peak);
}

float Serotonin::getExcitabilityModulator() const {
    // Serotonin generally reduces excitability, promoting calmness
    return std::max(0.0f, 1.0f - pImpl->level * 0.6f);
}

float Serotonin::getAttentionModulator() const {
    // Serotonin modulates attention through mood and impulsivity control
    return pImpl->impulsivityControl;
}

float Serotonin::getMemoryModulator() const {
    // Serotonin promotes long-term memory consolidation
    return 0.5f + pImpl->socialBonding * 0.5f;
}

void Serotonin::regulateMood(float moodLevel) {
    // Direct mood regulation
    pImpl->mood = moodLevel;
    pImpl->level = std::clamp(moodLevel + 0.5f, pImpl->baseline, pImpl->peak);
}

void Serotonin::controlImpulsivity(float strength) {
    // Reduce impulsivity
    pImpl->impulsivityControl = std::max(0.0f, pImpl->impulsivityControl - strength);
}

} // namespace nlm