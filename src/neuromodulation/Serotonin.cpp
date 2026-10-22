// Serotonin: Mood, impulsivity, and social behavior neuromodulator
// Biological role: Regulates mood, inhibits behavioral inhibition, affects social behavior

#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodModulation;
    float impulseControl;
    float socialBehavior;
    float decayRate;
    float peak;
    
    // Emotional state
    float currentMood;
    float moodTarget;
    float emotionalStability;
    
    // Behavioral control
    float currentInhibition;
    float inhibitionTarget;
    float riskTaking;
    
    // Social state
    float socialCohesion;
    float socialReadiness;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , moodModulation(0.6f)
        , impulseControl(0.4f)
        , socialBehavior(0.5f)
        , decayRate(0.12f)
        , peak(1.0f)
        , currentMood(0.5f)
        , moodTarget(0.5f)
        , emotionalStability(0.5f)
        , currentInhibition(0.5f)
        , inhibitionTarget(0.5f)
        , riskTaking(0.5f)
        , socialCohesion(0.5f)
        , socialReadiness(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(class Brain* brain) {
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
    // 5-HT modulates plasticity related to mood and social learning
    return 0.5f + 0.5f * pImpl->level * pImpl->moodModulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Update mood
    pImpl->moodTarget = 0.5f + 0.5f * pImpl->level;
    pImpl->currentMood += (pImpl->moodTarget - pImpl->currentMood) * 0.05f;
    
    // Update emotional stability
    pImpl->emotionalStability = 0.5f + 0.5f * pImpl->level;
    
    // Update impulse control
    pImpl->inhibitionTarget = 0.5f - 0.3f * pImpl->level;  // Higher 5-HT = more inhibition
    pImpl->currentInhibition += (pImpl->inhibitionTarget - pImpl->currentInhibition) * 0.05f;
    
    // Update risk taking (inverse relationship)
    pImpl->riskTaking = 0.8f - 0.6f * pImpl->level;
    
    // Update social behavior
    pImpl->socialCohesion = 0.5f + 0.5f * pImpl->level * pImpl->socialBehavior;
    pImpl->socialReadiness = 0.5f + 0.5f * pImpl->level;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::enhanceMood(float positiveStimulus) {
    // Boost mood based on positive experiences
    if (positiveStimulus > 0.0f) {
        pImpl->currentMood = std::min(1.0f, pImpl->currentMood + positiveStimulus * 0.2f);
    }
    
    if (pImpl->brain) {
        // Store mood in brain for integration
        if (auto* config = pImpl->brain->getConfig()) {
            config->set("mood_level", pImpl->currentMood);
            config->set("inhibition_level", pImpl->currentInhibition);
            config->set("social_cohesion", pImpl->socialCohesion);
        }
    }
}

void Serotonin::triggerReward(float rewardValue) {
    // Serotonin responds to reward differently than dopamine
    // More associated with satiety and long-term mood regulation
    float moodBoost = rewardValue * 0.3f * pImpl->moodModulation;
    pImpl->currentMood = std::min(1.0f, pImpl->currentMood + moodBoost);
}

float Serotonin::getMoodLevel() const {
    return pImpl->currentMood;
}

float Serotonin::getInhibitionLevel() const {
    return pImpl->currentInhibition;
}

float Serotonin::getRiskTakingLevel() const {
    return pImpl->riskTaking;
}

float Serotonin::getSocialCohesionLevel() const {
    return pImpl->socialCohesion;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->currentMood = 0.5f;
    pImpl->moodTarget = 0.5f;
    pImpl->emotionalStability = 0.5f;
    pImpl->currentInhibition = 0.5f;
    pImpl->inhibitionTarget = 0.5f;
    pImpl->riskTaking = 0.5f;
    pImpl->socialCohesion = 0.5f;
    pImpl->socialReadiness = 0.5f;
}

} // namespace nlm