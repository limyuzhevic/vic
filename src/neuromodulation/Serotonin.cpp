#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodState;
    float impulsivityFactor;
    float socialBehaviorModifier;
    float rapheNucleiActivity;
    std::vector<float> moodHistory;
    Timestamp lastPositiveEvent;
    Timestamp lastNegativeEvent;
    std::vector<float> firingHistory;
    
    Impl() : level(0.5f), baseline(0.2f), peak(1.5f), decayRate(0.04f), releaseRate(1.0f),
             moodState(0.5f), impulsivityFactor(1.0f), socialBehaviorModifier(1.0f),
             rapheNucleiActivity(0.0f), lastPositiveEvent(0), lastNegativeEvent(0) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin system initialized - mood and social behavior modulation ready");
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.5f);
    pImpl->moodState = std::min(1.0f, std::max(0.0f, (pImpl->level - 0.2f) / 1.3f));
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects plasticity differently depending on concentration:
    // Low levels: promote extinction learning, reduce impulsivity
    // High levels: promote mood stability, enhance social learning
    float mood = pImpl->moodState;
    
    if (mood < 0.3f) {
        // Low serotonin - promotes adaptive extinction of aversive memories
        return 0.8f + mood * 0.4f;  // Moderate plasticity for extinction
    } else if (mood > 0.7f) {
        // High serotonin - promotes mood stability and social learning
        return 1.0f - (mood - 0.7f) * 0.3f;  // Slightly reduced plasticity for stability
    } else {
        // Mid-range - balanced plasticity
        return 1.0f;
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Decay with circadian and homeostatic influences
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Maintain mood history for behavioral modulation
    pImpl->moodHistory.push_back(pImpl->moodState);
    if (pImpl->moodHistory.size() > 200) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
    
    // Maintain firing history for plasticity modulation
    if (pImpl->level > 0.1f) {
        pImpl->firingHistory.push_back(pImpl->level);
        if (pImpl->firingHistory.size() > 100) {
            pImpl->firingHistory.erase(pImpl->firingHistory.begin());
        }
    }
    
    // Update raphe nuclei activity based on mood state
    pImpl->rapheNucleiActivity = pImpl->moodState * 0.5f; // Biological coupling
}

void Serotonin::signalPositiveOutcome(float rewardMagnitude, float socialContext) {
    // Serotonin release in response to positive outcomes, especially in social contexts
    // Enhances mood stability and social bonding
    float positiveBoost = rewardMagnitude * 0.6f + socialContext * 0.4f;
    float burstAmount = positiveBoost * pImpl->releaseRate * 0.8f;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    pImpl->lastPositiveEvent = pImpl->lastPositiveEvent + dt; // Should use proper Timestamp
    
    // Social context bonus
    if (socialContext > 0.7f) {
        pImpl->socialBehaviorModifier = 1.2f;  // Enhanced social behavior
    }
    
    NLM_LOG_INFO("Serotonin increase following positive outcome");
}

void Serotonin::signalNegativeOutcome(float punishmentSeverity, float isolationLevel) {
    // Moderate serotonin response to negative events (prevents overreaction)
    // Supports emotional regulation and resilience
    float negativeImpact = punishmentSeverity * 0.7f + isolationLevel * 0.3f;
    float reductionAmount = negativeImpact * pImpl->decayRate * 1.5f;
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - reductionAmount);
    pImpl->lastNegativeEvent = pImpl->lastNegativeEvent + dt; // Should use proper Timestamp
    
    // Isolation penalty
    if (isolationLevel > 0.5f) {
        pImpl->socialBehaviorModifier = 0.8f;  // Reduced social engagement
    }
    
    NLM_LOG_INFO("Serotonin decrease following negative outcome");
}

void Serotonin::enhanceSocialBonding(float socialCohesion) {
    // Enhance social behavior through serotonin modulation
    // Supports group cohesion and social learning
    float bondingBoost = socialCohesion * pImpl->socialBehaviorModifier * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondingBoost);
    pImpl->socialBehaviorModifier = 1.3f;  // Further enhance social behavior
}

void Serotonin::reduceImpulsivity(float cognitiveLoad, float stressLevel) {
    // Reduce impulsivity through serotonin modulation
    // Supports goal-directed behavior over habitual responses
    float reductionFactor = (cognitiveLoad * 0.6f + stressLevel * 0.4f) * 0.2f;
    float levelReduction = reductionFactor * pImpl->releaseRate;
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - levelReduction);
    pImpl->impulsivityFactor = 0.8f + pImpl->level * 0.2f;  // Lower level = lower impulsivity
}

void Serotonin::modulateSleepCycle(float sleepPressure) {
    // Serotonin modulates sleep-wake cycle
    // Higher levels promote wakefulness
    float wakePromotion = (1.0f - sleepPressure) * pImpl->releaseRate * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + wakePromotion);
}

} // namespace nlm