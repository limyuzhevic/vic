#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <chrono>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float predictionError;
    TimestepDuration lastUpdateTime;
    float burstFrequency;
    float adaptiveBaseline;
    
    Impl() : level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.15f), 
             releaseRate(2.0f), learningRate(0.1f), predictionError(0.0f),
             lastUpdateTime(0), burstFrequency(0.5f), adaptiveBaseline(0.02f) {}
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Dopamine system initialized");
}

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
    // Enhanced dopamine-modulated plasticity factor
    // Based on experimental neuroscience findings
    float phasicComponent = pImpl->level * 0.8f;
    float tonicComponent = pImpl->adaptiveBaseline * 0.2f;
    
    // Non-linear relationship: moderate dopamine maximizes plasticity
    return phasicComponent * (2.0f - phasicComponent) + tonicComponent;
}

void Dopamine::update(TimestepDuration dt) {
    if (dt <= 0) return;
    
    TimestepDuration timeSinceUpdate = dt - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = dt;
    
    // Decay towards adaptive baseline with learning
    float decayFactor = 1.0f - pImpl->decayRate * static_cast<float>(timeSinceUpdate);
    pImpl->level = pImpl->adaptiveBaseline + (pImpl->level - pImpl->adaptiveBaseline) * decayFactor;
    
    // Add prediction error modulation
    if (std::abs(pImpl->predictionError) > 0.001f) {
        pImpl->level += pImpl->predictionError * pImpl->learningRate;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
        pImpl->predictionError = 0.0f; // Reset after application
    }
    
    // Adaptive baseline learning
    pImpl->adaptiveBaseline = 0.01f + pImpl->level * 0.1f;
}

void Dopamine::signalReward(float reward) {
    if (reward > 0.0f) {
        // Reward prediction error: actual reward - expected reward (baseline)
        float predictionError = reward - pImpl->adaptiveBaseline;
        
        // Phasic dopamine burst proportional to reward prediction error
        float burst = std::min(pImpl->peak, predictionError * pImpl->releaseRate);
        
        // Exponential decay of burst for more realistic dynamics
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Increase adaptive baseline for next predictions
        pImpl->adaptiveBaseline = std::max(pImpl->adaptiveBaseline, 
                                         pImpl->adaptiveBaseline + reward * 0.01f);
        
        NLM_LOG_TRACE("Dopamine burst: reward={}, level={}", reward, pImpl->level);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Handle prediction error signals directly
    if (std::abs(error) > 0.001f) {
        // Positive error (better than expected): reward prediction error
        // Negative error (worse than expected): punishment prediction error
        
        if (error > 0.0f) {
            // Reward prediction error - increase dopamine
            pImpl->predictionError += error * pImpl->learningRate;
            pImpl->adaptiveBaseline = std::max(pImpl->adaptiveBaseline, 
                                             pImpl->adaptiveBaseline + error * 0.005f);
        } else {
            // Punishment prediction error - decrease dopamine
            pImpl->level += error * pImpl->learningRate * 0.5f; // Smaller effect
            pImpl->level = std::max(0.0f, pImpl->level);
            pImpl->adaptiveBaseline = std::max(pImpl->adaptiveBaseline, 
                                             pImpl->adaptiveBaseline - std::abs(error) * 0.002f);
        }
        
        NLM_LOG_TRACE("Dopamine prediction error: error={}, level={}", error, pImpl->level);
    }
}

void Dopamine::setBaseline(float baseline) {
    pImpl->adaptiveBaseline = std::clamp(baseline, 0.0f, 0.5f);
}

float Dopamine::getBaseline() const {
    return pImpl->adaptiveBaseline;
}

void Dopamine::setBurstFrequency(float frequency) {
    pImpl->burstFrequency = std::max(0.01f, std::min(frequency, 10.0f));
}

float Dopamine::getBurstFrequency() const {
    return pImpl->burstFrequency;
}

void Dopamine::reset() {
    pImpl->level = pImpl->adaptiveBaseline;
    pImpl->predictionError = 0.0f;
}

} // namespace nlm