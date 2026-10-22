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
    float moodLevel;  // For mood and impulsivity regulation
    float socialBonding;  // For social behavior
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.0f)
        , moodLevel(0.0f)
        , socialBonding(0.0f) {}
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
    // Update mood and social bonding based on serotonin level
    pImpl->moodLevel = level * 1.5f;  // Strong mood effect
    pImpl->socialBonding = level * 0.8f;  // Social behavior modulation
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood, impulsivity, and social behavior
    // Moderate levels enhance learning, high levels may impair it
    float optimalLevel = 0.4f;
    float deviation = std::abs(pImpl->level - optimalLevel);
    
    // Inverted U-shaped function with peak at moderate levels
    float plasticity = 1.2f - deviation * 2.0f;  // Range: 1.2 to -0.8
    return std::max(0.2f, plasticity);  // Minimum 0.2
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // If brain is available, update mood and social effects
    if (pImpl->brain) {
        // Apply mood modulation
        if (pImpl->moodLevel > 0.3f) {
            // Positive mood enhances certain types of learning
            if (pImpl->brain->getWorkingMemory()) {
                // Happy mood improves working memory performance
                float moodBoost = pImpl->moodLevel * 0.2f;
                pImpl->brain->getWorkingMemory()->strengthenMemory(1.0f + moodBoost);
            }
        }
        
        // Social behavior modulation
        if (pImpl->socialBonding > 0.2f) {
            // Enhanced social bonding promotes cooperation and trust
            // This would affect decision making and social behavior
            if (pImpl->brain->getDopamine()) {
                // Increase reward for social interactions
                float currentDA = pImpl->brain->getDopamine()->getLevel();
                pImpl->brain->getDopamine()->setLevel(
                    std::min(1.0f, currentDA + pImpl->socialBonding * 0.3f));
            }
        }
        
        // Reduce impulsivity at moderate levels
        if (pImpl->level > 0.6f) {
            // High serotonin reduces impulsive behavior
            // This would affect action selection and decision making
        }
    }
}

void Serotonin::signalMood(float moodStrength) {
    // Signal mood change (positive or negative)
    float moodBoost = moodStrength * pImpl->releaseRate;
    float newLevel = std::min(pImpl->peak, pImpl->level + moodBoost);
    setLevel(newLevel);
}

void Serotonin::signalSocialBonding(float bondingStrength) {
    // Signal social connection or isolation
    if (bondingStrength > 0.0f) {
        float bondingBoost = bondingStrength * pImpl->releaseRate * 1.2f;
        float newLevel = std::min(pImpl->peak, pImpl->level + bondingBoost);
        setLevel(newLevel);
        
        // Social bonding enhances memory consolidation
        if (pImpl->brain && pImpl->brain->getEpisodicMemory()) {
            // Store social experience in episodic memory
            pImpl->brain->getEpisodicMemory()->setSocialBondingFactor(1.0f + bondingStrength);
        }
    } else {
        // Isolation reduces serotonin
        setLevel(pImpl->level + bondingStrength * 0.5f);  // Negative bonding reduces level
    }
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to reward, especially social rewards
    float rewardBoost = reward * pImpl->releaseRate * 0.7f;
    float newLevel = std::min(pImpl->peak, pImpl->level + rewardBoost);
    setLevel(newLevel);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getSocialBondingFactor() const {
    return pImpl->socialBonding;
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - mood and social behavior regulation");
}

} // namespace nlm
