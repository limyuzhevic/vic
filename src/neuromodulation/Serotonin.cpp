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
    float rewardSensitivity;
    float punishmentSensitivity;
    float socialLearning;
    
    // Mood and affect state
    float currentMood;
    float affectValue;
    float rewardResponse;
    
    // Behavioral regulation
    float impulseControl;
    float behavioralFlexibility;
    float riskTaking;
    
    // Social state
    float socialBonding;
    float socialHierarchy;
    float cooperationTendency;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(0.8f)
        , decayRate(0.18f)
        , rewardSensitivity(0.4f)
        , punishmentSensitivity(0.3f)
        , socialLearning(0.2f)
        , currentMood(0.5f)
        , affectValue(0.5f)
        , rewardResponse(0.5f)
        , impulseControl(0.5f)
        , behavioralFlexibility(0.5f)
        , riskTaking(0.5f)
        , socialBonding(0.5f)
        , socialHierarchy(0.5f)
        , cooperationTendency(0.5f) {}
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

float Serotonin::getMoodRegulation() const {
    // Higher serotonin improves mood stability
    return 0.3f + pImpl->level * 0.7f;
}

float Serotonin::getImpulseControl() const {
    // Serotonin increases impulse control
    return 0.2f + pImpl->level * 0.8f;
}

float Serotonin::getSocialBehaviorModulation() const {
    // Serotonin influences social behavior
    return 0.3f + pImpl->level * 0.7f;
}

void Serotonin::update(TimestepDuration dt, float rewardHistory, float punishmentHistory, float socialInteraction) {
    pImpl->currentMood = pImpl->level;
    
    // Reward history drives serotonin
    float rewardDriven = pImpl->rewardSensitivity * rewardHistory;
    
    // Punishment history reduces serotonin
    float punishmentDriven = -pImpl->punishmentSensitivity * punishmentHistory;
    
    // Social interactions influence serotonin
    float socialDriven = pImpl->socialLearning * socialInteraction;
    
    // Combined input
    float input = rewardDriven + punishmentDriven + socialDriven;
    
    // Smooth update with adaptation
    pImpl->level += (input - pImpl->level) * 0.12f;
    
    // Decay with homeostatic regulation
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update affect and mood
    pImpl->affectValue = 0.3f + pImpl->level * 0.7f;
    pImpl->rewardResponse = 0.4f + pImpl->level * 0.6f;
    
    // Update behavioral regulation
    pImpl->impulseControl = 0.1f + pImpl->level * 0.9f;
    pImpl->behavioralFlexibility = 0.3f + pImpl->level * 0.7f;
    pImpl->riskTaking = 0.4f - pImpl->level * 0.5f;  // High serotonin reduces risk-taking
    
    // Update social behavior
    pImpl->socialBonding = 0.2f + pImpl->level * 0.8f;
    pImpl->socialHierarchy = 0.3f + pImpl->level * 0.7f;
    pImpl->cooperationTendency = 0.3f + pImpl->level * 0.7f;
}

void Serotonin::signalOutcome(float outcomeValue) {
    // Positive outcomes increase serotonin
    if (outcomeValue > 0.3f) {
        float boost = outcomeValue * pImpl->rewardSensitivity;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * 0.5f);
    } 
    // Negative outcomes can decrease serotonin
    else if (outcomeValue < -0.3f) {
        float reduction = std::abs(outcomeValue) * pImpl->punishmentSensitivity;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - reduction * 0.4f);
    }
}

void Serotonin::signalSocial(float interactionValue) {
    // Social interactions modulate serotonin
    float boost = interactionValue * pImpl->socialLearning;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * 0.3f);
    
    // Positive social interactions enhance bonding
    if (interactionValue > 0.5f) {
        pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + interactionValue * 0.1f);
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentMood = 0.5f;
    pImpl->affectValue = 0.5f;
    pImpl->rewardResponse = 0.5f;
    pImpl->impulseControl = 0.5f;
    pImpl->behavioralFlexibility = 0.5f;
    pImpl->riskTaking = 0.5f;
    pImpl->socialBonding = 0.5f;
    pImpl->socialHierarchy = 0.5f;
    pImpl->cooperationTendency = 0.5f;
}

} // namespace nlm