#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float rewardHistory;
    float predictionErrorHistory;
    float plasticityFactor;
    
    // Reward prediction error computation
    float rewardPredictionError;
    float expectedReward;
    
    Impl() : level(0.0f), rewardHistory(0.0f), predictionErrorHistory(0.0f), 
             plasticityFactor(1.0f), rewardPredictionError(0.0f), expectedReward(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, -1.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: positive increases, negative decreases
    return std::clamp(1.0f + pImpl->level * 0.5f, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level *= 0.95f;
    
    // Integrate prediction error
    pImpl->rewardPredictionError = pImpl->level;
    
    // Update expected reward
    pImpl->expectedReward = 0.95f * pImpl->expectedReward + 0.05f * pImpl->level;
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardHistory = reward;
    
    // Compute prediction error
    pImpl->rewardPredictionError = reward - pImpl->expectedReward;
    
    // Update dopamine level based on prediction error
    setLevel(pImpl->rewardPredictionError);
    
    NLM_LOG_INFO(std::string("Dopamine: Reward signal = ") + std::to_string(reward) + 
                " Prediction error = " + std::to_string(pImpl->rewardPredictionError));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->rewardPredictionError = error;
    setLevel(error);
    
    NLM_LOG_INFO(std::string("Dopamine: Prediction error = ") + std::to_string(error));
}

// Acetylcholine Implementation - for attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float attentionBias;
    float memoryConsolidationRate;
    
    // For memory consolidation tracking
    std::vector<float> memoryTraceStabilities;
    
    Impl() : level(0.0f), attentionBias(0.0f), memoryConsolidationRate(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // ACh affects attention and memory
    pImpl->attentionBias = level * 2.0f;
    pImpl->memoryConsolidationRate = 0.3f + level * 0.7f;
    
    NLM_LOG_INFO(std::string("Acetylcholine: Level = ") + std::to_string(level) + 
                " Attention bias = " + std::to_string(pImpl->attentionBias));
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances both LTP and LTD
    return std::clamp(1.0f + pImpl->level * 0.3f, 0.8f, 1.8f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay level over time
    pImpl->level *= 0.9f;
    
    // Update attention bias
    pImpl->attentionBias = pImpl->level * 2.0f;
}

// Norepinephrine Implementation - for arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilanceLevel;
    float locusCoeruleusGain;
    
    // For signal detection and noise reduction
    float signalToNoiseRatio;
    
    Impl() : level(0.0f), arousalLevel(0.0f), vigilanceLevel(0.0f), 
             locusCoeruleusGain(1.0f), signalToNoiseRatio(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    
    // NE affects arousal and vigilance
    pImpl->arousalLevel = std::min(pImpl->level, 1.0f);
    pImpl->vigilanceLevel = std::max(0.0f, pImpl->level - 1.0f);
    
    // Update signal-to-noise ratio
    pImpl->signalToNoiseRatio = 1.0f + pImpl->arousalLevel * 2.0f;
    
    NLM_LOG_INFO(std::string("Norepinephrine: Level = ") + std::to_string(level) + 
                " Arousal = " + std::to_string(pImpl->arousalLevel) + 
                " Vigilance = " + std::to_string(pImpl->vigilanceLevel));
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity with complex effects
    return std::clamp(1.0f + pImpl->arousalLevel * 0.5f - pImpl->vigilanceLevel * 0.2f, 0.5f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics are complex - gradual decay
    pImpl->level *= 0.97f;
    
    // Update derived measures
    pImpl->arousalLevel = std::min(pImpl->level, 1.0f);
    pImpl->vigilanceLevel = std::max(0.0f, pImpl->level - 1.0f);
    pImpl->signalToNoiseRatio = 1.0f + pImpl->arousalLevel * 2.0f;
}

// Serotonin Implementation - for mood, impulsivity, and social behavior
struct Serotonin::Impl {
    float level;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorModulator;
    
    // For behavioral state modulation
    float behavioralState;
    float rewardSensitivity;
    
    Impl() : level(0.0f), moodLevel(0.5f), impulsivityLevel(0.5f), 
             socialBehaviorModulator(1.0f), behavioralState(0.5f), rewardSensitivity(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    
    // 5-HT affects mood and impulsivity
    pImpl->moodLevel = 0.3f + pImpl->level * 0.35f; // 0.3 to 1.0
    pImpl->impulsivityLevel = 0.5f + pImpl->level * 0.3f; // 0.5 to 1.4
    pImpl->socialBehaviorModulator = 0.5f + pImpl->level * 0.5f; // 0.5 to 1.5
    
    // Update reward sensitivity
    pImpl->rewardSensitivity = 0.8f + pImpl->level * 0.4f; // 0.8 to 2.0
    
    NLM_LOG_INFO(std::string("Serotonin: Level = ") + std::to_string(level) + 
                " Mood = " + std::to_string(pImpl->moodLevel) + 
                " Impulsivity = " + std::to_string(pImpl->impulsivityLevel));
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT typically reduces plasticity (stabilizes memories)
    return std::clamp(1.0f - pImpl->level * 0.2f, 0.5f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics
    pImpl->level *= 0.96f;
    
    // Update derived measures
    pImpl->moodLevel = 0.3f + pImpl->level * 0.35f;
    pImpl->impulsivityLevel = 0.5f + pImpl->level * 0.3f;
    pImpl->socialBehaviorModulator = 0.5f + pImpl->level * 0.5f;
}

} // namespace nlm