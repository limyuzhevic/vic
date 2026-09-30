#pragma once

#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>
#include <string>
#include <unordered_map>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and signal-to-noise ratio
    float arousalLevel;
    float arousalThreshold;
    float noiseLevel;
    
    // Attention and vigilance
    float vigilanceLevel;
    float attentionSelectiveGain;
    std::string attentionalFocus;
    
    // Memory strengthening
    std::unordered_map<float, float> memoryTraces; // traceId -> strength
    float memoryDecayRate;
    
    // Stress response
    float stressLevel;
    bool stressResponseActive;
    std::string stressContext;
    
    // Learning and adaptation
    float learningRate;
    float adaptationRate;
    std::vector<float> recentPredictionErrors;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(1.0f), arousalThreshold(0.5f), noiseLevel(0.1f),
             vigilanceLevel(1.0f), attentionSelectiveGain(1.0f),
             stressLevel(0.0f), stressResponseActive(false),
             learningRate(0.01f), adaptationRate(0.005f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity through arousal and attention
    float plasticityFactor = 1.0f + pImpl->arousalLevel * 0.3f;
    plasticityFactor *= pImpl->vigilanceLevel;
    return std::min(2.0f, plasticityFactor);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Enhanced NE dynamics with arousal modulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update recent prediction errors
    if (pImpl->recentPredictionErrors.size() > 10) {
        pImpl->recentPredictionErrors.erase(pImpl->recentPredictionErrors.begin());
    }
}

// Enhanced arousal and signal-to-noise ratio functions
void Norepinephrine::setArousalThreshold(float threshold) {
    pImpl->arousalThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Norepinephrine::setNoiseLevel(float noise) {
    pImpl->noiseLevel = std::clamp(noise, 0.0f, 1.0f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getSignalToNoiseRatio() const {
    return std::max(0.1f, 1.0f - pImpl->noiseLevel);
}

// Enhanced attention and vigilance functions
void Norepinephrine::setVigilanceTarget(float target) {
    pImpl->vigilanceLevel = std::clamp(target, 0.0f, 2.0f);
}

void Norepinephrine::setAttentionalFocus(const std::string& focus) {
    pImpl->attentionalFocus = focus;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getAttentionSelectiveGain() const {
    return pImpl->attentionSelectiveGain;
}

void Norepinephrine::setAttentionSelectiveGain(float gain) {
    pImpl->attentionSelectiveGain = std::clamp(gain, 0.0f, 2.0f);
}

// Enhanced memory strengthening functions
void Norepinephrine::strengthenMemoryTrace(float traceId, float strength) {
    pImpl->memoryTraces[traceId] = std::clamp(strength, 0.0f, 1.0f);
}

float Norepinephrine::getMemoryStrength(float traceId) const {
    auto it = pImpl->memoryTraces.find(traceId);
    if (it != pImpl->memoryTraces.end()) {
        return it->second;
    }
    return 0.0f;
}

void Norepinephrine::setMemoryDecayRate(float decayRate) {
    pImpl->memoryDecayRate = std::clamp(decayRate, 0.0f, 0.5f);
}

// Enhanced stress response functions
bool Norepinephrine::isStressResponse(float stressor) const {
    return stressor > pImpl->arousalThreshold && pImpl->stressLevel > 0.5f;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::triggerStressResponse(float intensity, const std::string& context) {
    pImpl->stressResponseActive = true;
    pImpl->stressLevel = std::clamp(intensity, 0.0f, 1.0f);
    pImpl->stressContext = context;
    
    // Modulate arousal and attention during stress
    pImpl->arousalLevel = 1.5f;
    pImpl->attentionSelectiveGain = 2.0f;
}

// Enhanced learning and adaptation functions
float Norepinephrine::getLearningRate() const {
    return pImpl->learningRate;
}

float Norepinephrine::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

void Norepinephrine::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

void Norepinephrine::updateAdaptation(const std::vector<float>& predictionErrors) {
    pImpl->recentPredictionErrors = predictionErrors;
    
    if (!predictionErrors.empty()) {
        float avgError = 0.0f;
        for (float error : predictionErrors) {
            avgError += error;
        }
        avgError /= static_cast<float>(predictionErrors.size());
        
        // Adapt learning rate based on prediction error magnitude
        pImpl->learningRate = std::clamp(0.01f + avgError * 0.05f, 0.001f, 0.1f);
        pImpl->adaptationRate = std::clamp(0.005f + avgError * 0.02f, 0.001f, 0.05f);
    }
}

// Cross-system integration functions
void Norepinephrine::integrateWithArousalSystems(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 2.0f);
}

void Norepinephrine::integrateWithMemorySystems(float strength) {
    // Find or create a memory trace and strengthen it
    if (!pImpl->memoryTraces.empty()) {
        float traceId = pImpl->memoryTraces.begin()->first;
        strengthenMemoryTrace(traceId, strength);
    }
}

void Norepinephrine::integrateWithAttentionalSystems(float vigilance) {
    setVigilanceTarget(vigilance);
}

void Norepinephrine::integrateWithLearningSystems(float adaptation) {
    setAdaptationRate(std::clamp(adaptation, 0.001f, 0.05f));
    
    // Create sample prediction errors for adaptation
    std::vector<float> errors;
    errors.push_back(adaptation * 0.8f);
    errors.push_back((1.0f - adaptation) * 0.4f);
    updateAdaptation(errors);
}

} // namespace nlm
