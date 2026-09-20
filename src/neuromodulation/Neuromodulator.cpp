#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <map>
#include <unordered_map>

namespace nlm {

// Helper function to get current time
static double getCurrentTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}

// Acetylcholine: Attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    double lastUpdateTime;
    double attentionBias;
    
    Impl() : level(0.0f), baseline(0.0f), peak(0.3f), decayRate(0.05f), releaseRate(0.5f), 
             lastUpdateTime(0.0), attentionBias(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related memories
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionBias;
}

void Acetylcholine::update(TimestepDuration dt) {
    double currentTime = getCurrentTime();
    double deltaTime = currentTime - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = currentTime;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(deltaTime));
    
    // Modulate attention based on level
    pImpl->attentionBias = 1.0f + 2.0f * pImpl->level;
}

void Acetylcholine::signalAttention(boost::string_view focusArea) {
    // Boost ACh when attending to important areas
    float boost = 0.5f;
    if (focusArea == "memory" || focusArea == "learning") {
        boost = 0.8f;
    } else if (focusArea == "sensory" || focusArea == "perceptual") {
        boost = 0.6f;
    }
    
    setLevel(std::min(1.0f, getLevel() + boost));
}

// Norepinephrine: Arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    double lastUpdateTime;
    float arousalLevel;
    float locusCoeruleusGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(0.4f), decayRate(0.03f), releaseRate(0.7f), 
             lastUpdateTime(0.0), arousalLevel(1.0f), locusCoeruleusGain(1.0f) {}
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
    
    // Update arousal based on NE level
    pImpl->arousalLevel = 1.0f + 3.0f * level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal
    return 0.3f + 0.7f * pImpl->level * pImpl->arousalLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    double currentTime = getCurrentTime();
    double deltaTime = currentTime - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = currentTime;
    
    // Decay towards baseline with time constant
    float targetLevel = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(deltaTime));
    pImpl->level = 0.9f * targetLevel + 0.1f * pImpl->level; // Smoothing
    
    // Locus coeruleus burst firing when unexpected events occur
    pImpl->locusCoeruleusGain = 1.0f + 2.0f * pImpl->level;
}

void Norepinephrine::signalArousal(boost::string_view eventType, float intensity) {
    // Boost NE for surprising or intense events
    float boost = 0.2f * intensity;
    if (eventType == "novel" || eventType == "unexpected") {
        boost *= 2.0f;
    } else if (eventType == "reward" || eventType == "punishment") {
        boost *= 1.5f;
    }
    
    setLevel(std::min(1.0f, getLevel() + boost));
}

// Serotonin: Mood, impulsivity, and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    double lastUpdateTime;
    float socialBonding;
    float satietyLevel;
    
    Impl() : level(0.5f), baseline(0.2f), peak(1.0f), decayRate(0.08f), releaseRate(0.4f), 
             lastUpdateTime(0.0), socialBonding(1.0f), satietyLevel(0.5f) {}
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
    
    // Update social bonding and satiety
    pImpl->socialBonding = 1.0f + 2.0f * pImpl->level;
    pImpl->satietyLevel = 0.3f + 0.7f * pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin decreases plasticity for safety (anti-learning)
    return std::max(0.1f, 1.0f - 0.8f * pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    double currentTime = getCurrentTime();
    double deltaTime = currentTime - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = currentTime;
    
    // Complex dynamics: decay but can be boosted by positive social interactions
    float targetLevel = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(deltaTime));
    pImpl->level = 0.95f * targetLevel + 0.05f * pImpl->level; // Gradual adjustment
}

void Serotonin::signalSocial(boost::string_view interactionType, float intensity) {
    // Boost 5-HT for positive social interactions
    float boost = 0.3f * intensity;
    if (interactionType == "social" || interactionType == "bond") {
        boost *= 2.0f;
    } else if (interactionType == "conflict" || interactionType == "threat") {
        boost *= -1.0f; // Decrease with negative social interactions
    }
    
    setLevel(std::max(0.0f, std::min(1.0f, getLevel() + boost)));
}

// Dopamine: Enhanced implementation with prediction error
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    double lastUpdateTime;
    float predictionError;
    float rewardPrediction;
    float temporalDifference;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             lastUpdateTime(0.0), predictionError(0.0f), rewardPrediction(0.0f), 
             temporalDifference(0.0f) {}
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
    // Enhanced plasticity modulation based on prediction error
    float predictionGain = 1.0f + 2.0f * std::abs(pImpl->predictionError);
    return 0.5f + 0.5f * pImpl->level * predictionGain;
}

void Dopamine::update(TimestepDuration dt) {
    double currentTime = getCurrentTime();
    double deltaTime = currentTime - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = currentTime;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(deltaTime));
    
    // Update prediction error history
    pImpl->predictionError = pImpl->rewardPrediction - pImpl->temporalDifference;
}

void Dopamine::signalReward(float reward) {
    // Burst response to unexpected rewards
    float predictionError = reward - pImpl->rewardPrediction;
    
    // Stronger response to unexpected rewards (prediction error)
    float boost = std::abs(predictionError) * pImpl->releaseRate;
    setLevel(std::min(pImpl->peak, getLevel() + boost));
    
    // Update reward prediction
    pImpl->rewardPrediction = 0.9f * pImpl->rewardPrediction + 0.1f * reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Respond to prediction errors
    pImpl->predictionError = error;
    
    // Update temporal difference
    pImpl->temporalDifference = error;
    
    // Moderate boost for prediction errors
    setLevel(std::min(pImpl->peak, getLevel() + std::abs(error) * 0.5f * pImpl->releaseRate));
}

void Dopamine::signalTemporalDifference(float td) {
    // Temporal difference learning signal
    pImpl->temporalDifference = td;
    pImpl->predictionError = td;
    
    // Stronger response for large prediction errors
    setLevel(std::min(pImpl->peak, getLevel() + std::abs(td) * 0.3f * pImpl->releaseRate));
}

} // namespace nlm
