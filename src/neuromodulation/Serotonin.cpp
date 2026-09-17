#include "Serotonin.hpp"
#include "../../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    // 5-HT dynamics parameters
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and affect parameters
    float moodScore;
    float satietyLevel;
    float impatience;
    float impulsivity;
    
    // Social behavior parameters
    float socialCohesion;
    float territoriality;
    float aggression;
    
    // State management
    float emotionalState;
    float socialBonding;
    std::vector<float> moodHistory;
    std::vector<float> socialHistory;
    
    // Learning-related parameters
    float riskAversion;
    float patience;
    float rewardSensitivity;
    
    // Random number generator for stochastic behavior
    std::mt19937 rng;
    
    Impl() : level(0.02f), baseline(0.02f), peak(0.4f), decayRate(0.04f), 
             releaseRate(1.0f), moodScore(0.5f), satietyLevel(0.5f),
             impatience(0.5f), impulsivity(0.5f), socialCohesion(0.5f),
             territoriality(0.5f), aggression(0.3f), emotionalState(0.5f),
             socialBonding(0.5f), riskAversion(0.5f), patience(0.5f),
             rewardSensitivity(0.7f) {
        // Initialize random generator
        std::random_device rd;
        rng.seed(rd());
    }
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
    // Serotonin modulates plasticity in complex, context-dependent ways
    float factor = 1.0f;
    
    if (pImpl->level < 0.1f) {
        // Low serotonin: increased impulsivity, faster learning
        factor = 1.2f + pImpl->level * 2.0f;
    } else if (pImpl->level < 0.3f) {
        // Moderate serotonin: balanced learning
        factor = 0.8f + pImpl->level * 2.0f;
    } else if (pImpl->level < 0.6f) {
        // High serotonin: improved mood, enhanced stable learning
        factor = 1.0f + pImpl->level * 1.0f;
    } else {
        // Very high serotonin: over-satiated, reduced exploration
        factor = 0.6f + pImpl->level * 0.5f;
    }
    
    // Apply mood modulation
    factor *= (1.0f + pImpl->moodScore * 0.3f);
    
    // Apply satiety modulation
    factor *= (1.0f - pImpl->satietyLevel * 0.2f);
    
    // Apply social bonding modulation
    factor *= (1.0f + pImpl->socialBonding * 0.2f);
    
    return factor;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update mood score based on level
    pImpl->moodScore = pImpl->moodScore * 0.9f + pImpl->level * 0.2f;
    
    // Update satiety
    pImpl->satietyLevel = pImpl->satietyLevel * 0.95f;
    
    // Update emotional state
    pImpl->emotionalState = pImpl->emotionalState * 0.9f + pImpl->level * 0.1f;
    
    // Update social bonding
    pImpl->socialBonding = pImpl->socialBonding * 0.95f;
    
    // Store history
    pImpl->moodHistory.push_back(pImpl->moodScore);
    if (pImpl->moodHistory.size() > 100) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
    
    pImpl->socialHistory.push_back(pImpl->socialBonding);
    if (pImpl->socialHistory.size() > 100) {
        pImpl->socialHistory.erase(pImpl->socialHistory.begin());
    }
}

void Serotonin::signalReward(float reward, float socialContext) {
    // Serotonin response to reward, modulated by social context
    if (reward > 0.0f) {
        // Reward response (tonic)
        float rewardResponse = reward * 0.8f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse);
        
        // Satiety increases with reward
        pImpl->satietyLevel = std::min(1.0f, pImpl->satietyLevel + reward * 0.05f);
        
        // Social context modulates response
        if (socialContext > 0.0f) {
            float socialMod = socialContext * 0.5f;
            pImpl->level = std::min(pImpl->peak, pImpl->level + socialMod);
            
            // Social bonding increases
            pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + socialContext * 0.1f);
        }
        
        // Update patience based on reward history
        pImpl->patience = std::min(1.0f, pImpl->patience + reward * 0.01f);
    }
}

void Serotonin::signalSocialEvent(float socialValue, float groupCohesion) {
    // Serotonin response to social events
    if (socialValue > 0.0f) {
        // Positive social event
        float socialResponse = socialValue * 1.2f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + socialResponse);
        
        // Increase social cohesion
        pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + socialValue * 0.1f);
        
        // Decrease aggression
        pImpl->aggression *= (1.0f - socialValue * 0.05f);
        
        // Increase territoriality (protect group)
        pImpl->territoriality = std::min(1.0f, pImpl->territoriality + socialValue * 0.05f);
    } else if (socialValue < 0.0f) {
        // Negative social event (conflict, exclusion)
        float conflictResponse = std::abs(socialValue) * 1.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + conflictResponse);
        
        // Decrease social cohesion
        pImpl->socialCohesion *= (1.0f - std::abs(socialValue) * 0.1f);
        
        // Increase aggression
        pImpl->aggression = std::min(1.0f, pImpl->aggression + std::abs(socialValue) * 0.1f);
        
        // Increase territoriality (defensive)
        pImpl->territoriality = std::min(1.0f, pImpl->territoriality + std::abs(socialValue) * 0.05f);
        
        // Decrease patience
        pImpl->patience *= (1.0f - std::abs(socialValue) * 0.05f);
    }
}

void Serotonin::signalStress(float stressLevel) {
    // Serotonin response to stress (serotonin often decreases under chronic stress)
    if (stressLevel > 0.5f) {
        // Chronic stress reduces serotonin
        float stressImpact = (stressLevel - 0.5f) * 0.8f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - stressImpact);
        
        // Increase impulsivity
        pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + stressLevel * 0.1f);
        
        // Decrease patience
        pImpl->patience *= (1.0f - stressLevel * 0.05f);
    } else {
        // Acute stress can actually increase serotonin in some contexts
        float stressImpact = stressLevel * 0.3f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressImpact);
    }
}

float Serotonin::getMoodScore() const {
    return pImpl->moodScore;
}

float Serotonin::getSatietyLevel() const {
    return pImpl->satietyLevel;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

float Serotonin::getAggression() const {
    return pImpl->aggression;
}

const std::vector<float>& Serotonin::getMoodHistory() const {
    return pImpl->moodHistory;
}

} // namespace nlm