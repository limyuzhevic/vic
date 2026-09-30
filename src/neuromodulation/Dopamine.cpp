#pragma once

#include "Neuromodulator.hpp"
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float workingMemoryStabilizer; // Enhanced: dopamine effect on working memory
    float noveltyAmplifier;        // Enhanced: novelty-based updates
    float explorationFactor;       // Enhanced: curiosity-based exploration
    float predictionErrorGain;     // Enhanced: prediction error response
    
    // Working memory modulation
    float memoryStabilityFactor;
    float memoryRehearsalBoost;
    
    // Attention and selection modulation
    float attentionGain;
    float selectionPriority;
    std::string attentionContext;
    
    // Prediction error tracking
    float predictionError;
    
    // Sensory and motor gating
    float sensoryGain;
    float motorGain;
    float sensoryFilter;
    float motorCommandFilter;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             workingMemoryStabilizer(0.0f), noveltyAmplifier(1.0f), explorationFactor(0.5f),
             predictionErrorGain(1.0f), memoryStabilityFactor(1.0f), memoryRehearsalBoost(1.0f),
             attentionGain(1.0f), selectionPriority(1.0f), attentionContext(""), predictionError(0.0f),
             sensoryGain(1.0f), motorGain(1.0f), sensoryFilter(0.5f), motorCommandFilter(0.5f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced: Real dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity, but too high can be destabilizing
    float plasticityFactor = 0.5f + 0.5f * pImpl->level;
    
    // Modulate plasticity based on working memory effects
    plasticityFactor *= pImpl->memoryStabilityFactor;
    
    // Apply attention modulation
    plasticityFactor *= pImpl->attentionGain;
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced: Real dopamine dynamics with working memory integration
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply working memory modulation to decay
    pImpl->level *= pImpl->workingMemoryStabilizer;
}

void Dopamine::signalReward(float reward) {
    // Enhanced: Real reward signaling with working memory modulation
    // Stronger reward strengthens working memory traces
    float memoryEffect = pImpl->memoryRehearsalBoost * 0.5f;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * (1.0f + memoryEffect));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced: Reward prediction error signaling with working memory effects
    // Prediction error modulates working memory consolidation
    float workingMemoryImpact = error * pImpl->predictionErrorGain * pImpl->noveltyAmplifier;
    
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Apply working memory modulation
    pImpl->memoryStabilityFactor = 1.0f + workingMemoryImpact * 0.2f;
    pImpl->memoryRehearsalBoost = 1.0f + std::abs(workingMemoryImpact) * 0.3f;
    pImpl->predictionError = error;
}

// Enhanced dopamine working memory functions
void Dopamine::stabilizeWorkingMemory(float stabilityFactor) {
    pImpl->workingMemoryStabilizer = std::clamp(stabilityFactor, 0.0f, 1.0f);
}

void Dopamine::amplifyNovelty(float noveltyFactor) {
    pImpl->noveltyAmplifier = std::clamp(noveltyFactor, 0.0f, 2.0f);
}

void Dopamine::enhanceExploration(float explorationFactor) {
    pImpl->explorationFactor = std::clamp(explorationFactor, 0.0f, 1.0f);
}

void Dopamine::setPredictionErrorGain(float gain) {
    pImpl->predictionErrorGain = std::clamp(gain, 0.0f, 2.0f);
}

float Dopamine::getWorkingMemoryStability() const {
    return pImpl->memoryStabilityFactor;
}

float Dopamine::getExplorationLevel() const {
    return pImpl->explorationFactor;
}

// Enhanced attention and selection functions
void Dopamine::setAttentionContext(const std::string& context) {
    pImpl->attentionContext = context;
}

float Dopamine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Dopamine::getSelectionPriority() const {
    return pImpl->selectionPriority;
}

void Dopamine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 2.0f);
}

void Dopamine::setSelectionPriority(float priority) {
    pImpl->selectionPriority = std::clamp(priority, 0.0f, 2.0f);
}

// Enhanced prediction error functions
float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::setPredictionError(float error) {
    pImpl->predictionError = error;
}

// Enhanced sensory and motor gating functions
void Dopamine::setSensoryFilter(float filter) {
    pImpl->sensoryFilter = std::clamp(filter, 0.0f, 1.0f);
}

void Dopamine::setMotorCommandFilter(float filter) {
    pImpl->motorCommandFilter = std::clamp(filter, 0.0f, 1.0f);
}

float Dopamine::getSensoryGain() const {
    return pImpl->sensoryGain;
}

float Dopamine::getMotorGain() const {
    return pImpl->motorGain;
}

void Dopamine::setSensoryGain(float gain) {
    pImpl->sensoryGain = std::clamp(gain, 0.0f, 2.0f);
}

void Dopamine::setMotorGain(float gain) {
    pImpl->motorGain = std::clamp(gain, 0.0f, 2.0f);
}

// Enhanced plasticity functions
float Dopamine::getEnhancedPlasticity() const {
    return 0.5f + 0.5f * pImpl->level * pImpl->noveltyAmplifier;
}

float Dopamine::getMetaPlasticity() const {
    return 1.0f + (pImpl->predictionError * 0.5f) * pImpl->explorationFactor;
}

// Cross-system integration functions
void Dopamine::integrateWithWorkingMemory(float stability, float recency) {
    stabilizeWorkingMemory(stability);
    setWorkingMemoryRecency(recency);
}

void Dopamine::integrateWithPredictionSystem(float error) {
    setPredictionError(error);
    setPredictionErrorGain(1.0f + std::abs(error) * 0.5f);
}

void Dopamine::integrateWithPlanningSystem(float value) {
    setSelectionPriority(std::clamp(value, 0.0f, 1.0f) * 2.0f);
}

void Dopamine::integrateWithEpisodicMemory(float strength) {
    setWorkingMemoryStability(std::clamp(strength, 0.0f, 1.0f));
}

void Dopamine::integrateWithConceptFormation(float novelty) {
    amplifyNovelty(std::clamp(novelty, 0.0f, 1.0f));
}

void Dopamine::setWorkingMemoryRecency(float recency) {
    // Store recency in memory rehearsal boost
    pImpl->memoryRehearsalBoost = std::clamp(recency, 0.0f, 1.0f);
}

} // namespace nlm
