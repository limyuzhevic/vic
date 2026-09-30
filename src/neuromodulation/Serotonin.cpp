#pragma once

#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>
#include <string>
#include <unordered_map>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and motivation
    float moodLevel;
    float motivationLevel;
    std::string moodTarget;
    std::string motivationTarget;
    
    // Decision making
    float decisionConfidence;
    float riskAversion;
    float explorationExploitationBalance;
    std::string decisionContext;
    
    // Behavioral flexibility
    float flexibilityLevel;
    std::unordered_map<std::string, std::string> behaviorChanges;
    float flexibilityTarget;
    
    // Learning rate modulation
    float learningRateModulation;
    float habituationRate;
    std::vector<std::pair<float, float>> learningHistory;
    
    // Punishment and avoidance
    float punishmentSensitivity;
    float avoidanceMotivations;
    std::unordered_map<std::string, float> avoidancePriorities;
    std::vector<std::pair<std::string, float>> punishmentHistory;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodLevel(1.0f), motivationLevel(1.0f), riskAversion(0.5f), flexibilityLevel(1.0f),
             learningRateModulation(1.0f), habituationRate(0.01f),
             punishmentSensitivity(0.5f), avoidanceMotivations(1.0f) {}
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
    // 5-HT modulates plasticity through mood and motivation
    float plasticityFactor = 1.0f;
    
    // Mood affects plasticity
    plasticityFactor += pImpl->moodLevel * 0.2f;
    
    // Motivation affects plasticity
    plasticityFactor += pImpl->motivationLevel * 0.1f;
    
    return std::min(2.0f, plasticityFactor);
}

void Serotonin::update(TimestepDuration dt) {
    // Enhanced 5-HT dynamics with mood and motivation integration
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update learning history
    if (pImpl->learningHistory.size() > 10) {
        pImpl->learningHistory.erase(pImpl->learningHistory.begin());
    }
    
    // Update punishment history
    if (pImpl->punishmentHistory.size() > 10) {
        pImpl->punishmentHistory.erase(pImpl->punishmentHistory.begin());
    }
}

// Enhanced mood and motivation functions
void Serotonin::setMoodTarget(float target) {
    pImpl->moodTarget = "mood_target_" + std::to_string(target);
    pImpl->moodLevel = std::clamp(target, 0.0f, 1.0f);
}

void Serotonin::setMotivationTarget(float target) {
    pImpl->motivationTarget = "motivation_target_" + std::to_string(target);
    pImpl->motivationLevel = std::clamp(target, 0.0f, 1.0f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getMotivationLevel() const {
    return pImpl->motivationLevel;
}

// Enhanced decision making functions
void Serotonin::setDecisionContext(const std::string& context) {
    pImpl->decisionContext = context;
}

float Serotonin::getDecisionConfidence() const {
    return pImpl->decisionConfidence;
}

float Serotonin::getRiskAversion() const {
    return pImpl->riskAversion;
}

float Serotonin::getExplorationExploitationBalance() const {
    return pImpl->explorationExploitationBalance;
}

void Serotonin::setDecisionConfidence(float confidence) {
    pImpl->decisionConfidence = std::clamp(confidence, 0.0f, 1.0f);
}

void Serotonin::setRiskAversion(float aversion) {
    pImpl->riskAversion = std::clamp(aversion, 0.0f, 1.0f);
}

void Serotonin::setExplorationExploitationBalance(float balance) {
    pImpl->explorationExploitationBalance = std::clamp(balance, 0.0f, 1.0f);
}

// Enhanced behavioral flexibility functions
bool Serotonin::isBehavioralFlexibilityRequired(float constraint) const {
    return constraint > pImpl->riskAversion && pImpl->flexibilityLevel < 0.5f;
}

float Serotonin::getFlexibilityLevel() const {
    return pImpl->flexibilityLevel;
}

void Serotonin::setFlexibilityTarget(float target) {
    pImpl->flexibilityTarget = std::clamp(target, 0.0f, 1.0f);
}

void Serotonin::recordBehaviorChange(const std::string& oldBehavior, const std::string& newBehavior) {
    pImpl->behaviorChanges[oldBehavior] = newBehavior;
    pImpl->flexibilityLevel = std::min(1.0f, pImpl->flexibilityLevel + 0.1f);
}

// Enhanced learning rate modulation functions
float Serotonin::getLearningRateModulation() const {
    return pImpl->learningRateModulation;
}

float Serotonin::getHabituationRate() const {
    return pImpl->habituationRate;
}

void Serotonin::setLearningRateModulation(float modulation) {
    pImpl->learningRateModulation = std::clamp(modulation, 0.5f, 2.0f);
}

void Serotonin::updateLearningRate(float experience, float outcome) {
    pImpl->learningHistory.push_back(std::make_pair(experience, outcome));
    
    // Update learning rate modulation based on experience and outcome
    float learningEffect = std::abs(outcome - experience);
    pImpl->learningRateModulation = 1.0f + learningEffect * 0.5f;
    
    // Apply habituation
    pImpl->habituationRate = std::min(0.1f, pImpl->habituationRate + learningEffect * 0.01f);
}

// Enhanced punishment and avoidance functions
float Serotonin::getPunishmentSensitivity() const {
    return pImpl->punishmentSensitivity;
}

float Serotonin::getAvoidanceMotivations() const {
    return pImpl->avoidanceMotivations;
}

void Serotonin::triggerPunishment(float intensity, const std::string& context) {
    pImpl->punishmentHistory.push_back(std::make_pair(context, intensity));
    pImpl->punishmentSensitivity = std::min(1.0f, pImpl->punishmentSensitivity + intensity * 0.1f);
    
    // Reduce motivation when punished
    pImpl->motivationLevel = std::max(0.0f, pImpl->motivationLevel - intensity * 0.2f);
}

void Serotonin::setAvoidancePriority(const std::string& goal, float priority) {
    pImpl->avoidancePriorities[goal] = std::clamp(priority, 0.0f, 1.0f);
}

// Cross-system integration functions
void Serotonin::integrateWithMoodSystems(float mood) {
    setMoodTarget(mood);
}

void Serotonin::integrateWithDecisionSystems(float confidence) {
    setDecisionConfidence(confidence);
}

void Serotonin::integrateWithBehavioralSystems(float flexibility) {
    setFlexibilityTarget(flexibility);
}

void Serotonin::integrateWithLearningSystems(float modulation) {
    setLearningRateModulation(modulation);
    
    // Create sample learning data for history
    float experience = 0.5f;
    float outcome = 0.5f + modulation * 0.3f;
    updateLearningRate(experience, outcome);
}

} // namespace nlm
