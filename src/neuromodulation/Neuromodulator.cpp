#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float attentionStrength;
    float memoryConsolidation;
    float decayRate;
    
    Impl()
        : level(0.0f)
        , attentionStrength(0.0f)
        , memoryConsolidation(0.0f)
        , decayRate(0.02f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const { return "ACh"; }

float Acetylcholine::getLevel() const { return pImpl->level; }

void Acetylcholine::setLevel(float level) { pImpl->level = std::max(0.0f, level); }

float Acetylcholine::getPlasticityFactor() const { return 1.0f + pImpl->level * 0.3f; }

void Acetylcholine::update(TimestepDuration dt) {
    // Decay over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Acetylcholine::getAttentionModulator() const {
    return 1.0f + pImpl->attentionStrength;
}

void Acetylcholine::boostAttention(float duration) {
    pImpl->attentionStrength = std::max(pImpl->attentionStrength, 0.5f * duration);
    NLM_LOG_INFO("Acetylcholine boosted attention by " + std::to_string(0.5f * duration));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    pImpl->memoryConsolidation = 0.8f;
    NLM_LOG_INFO("Acetylcholine enhanced memory consolidation");
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float arousalStrength;
    float sensoryGain;
    float decayRate;
    
    Impl()
        : level(0.0f)
        , arousalStrength(0.0f)
        , sensoryGain(0.0f)
        , decayRate(0.03f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const { return "NE"; }

float Norepinephrine::getLevel() const { return pImpl->level; }

void Norepinephrine::setLevel(float level) { pImpl->level = std::max(0.0f, level); }

float Norepinephrine::getPlasticityFactor() const { return 1.0f + pImpl->level * 0.2f; }

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Norepinephrine::getArousalModulator() const {
    return 1.0f + pImpl->arousalStrength;
}

void Norepinephrine::enhanceAlertness(float duration) {
    pImpl->arousalStrength = std::max(pImpl->arousalStrength, 0.3f * duration);
    NLM_LOG_INFO("Norepinephrine enhanced alertness by " + std::to_string(0.3f * duration));
}

void Norepinephrine::amplifySensoryGain() {
    pImpl->sensoryGain = 1.5f;
    NLM_LOG_INFO("Norepinephrine amplified sensory gain");
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float moodLevel;
    float learningRateModifier;
    float decayRate;
    
    Impl()
        : level(0.0f)
        , moodLevel(0.0f)
        , learningRateModifier(0.0f)
        , decayRate(0.015f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const { return "5-HT"; }

float Serotonin::getLevel() const { return pImpl->level; }

void Serotonin::setLevel(float level) { pImpl->level = std::max(0.0f, level); }

float Serotonin::getPlasticityFactor() const { return 1.0f + pImpl->level * 0.1f; }

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Serotonin::getLearningModulator() const {
    return 1.0f + pImpl->learningRateModifier;
}

void Serotonin::modulateMood(float moodLevel) {
    pImpl->moodLevel = std::max(0.0f, std::min(1.0f, moodLevel));
    pImpl->learningRateModifier = pImpl->moodLevel * 0.4f;
    NLM_LOG_INFO("Serotonin modulated mood to " + std::to_string(pImpl->moodLevel));
}

void Serotonin::adjustLearningRate() {
    // Adjust learning based on mood
    if (pImpl->moodLevel > 0.7f) {
        pImpl->learningRateModifier = 0.5f;
    } else if (pImpl->moodLevel < 0.3f) {
        pImpl->learningRateModifier = -0.2f;
    }
    NLM_LOG_INFO("Serotonin adjusted learning rate to " + std::to_string(1.0f + pImpl->learningRateModifier));
}

// Dopamine implementation
struct Dopamine::Impl {
    float level;
    float rewardSignal;
    float motivation;
    float workingMemoryBoost;
    float planningHorizon;
    float decayRate;
    
    Impl()
        : level(0.0f)
        , rewardSignal(0.0f)
        , motivation(0.0f)
        , workingMemoryBoost(0.0f)
        , planningHorizon(0.0f)
        , decayRate(0.01f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const { return "Dopamine"; }

float Dopamine::getLevel() const { return pImpl->level; }

void Dopamine::setLevel(float level) { pImpl->level = std::max(0.0f, level); }

float Dopamine::getPlasticityFactor() const {
    // Dopamine strongly enhances plasticity
    return 1.0f + pImpl->level * 0.8f;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Dopamine::getLearningModulator() const {
    return 1.0f + pImpl->level * 0.5f;
}

float Dopamine::getMemoryModulator() const {
    return 1.0f + pImpl->level * 0.3f;
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardSignal = reward;
    pImpl->level = std::max(pImpl->level, reward * 2.0f);
    NLM_LOG_INFO("Dopamine signaled reward: " + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->rewardSignal = error;
    // Error can boost or reduce dopamine
    if (error > 0.0f) {
        // Positive prediction error (better than expected) boosts dopamine
        pImpl->level = std::max(pImpl->level, error * 1.5f);
    } else {
        // Negative prediction error reduces dopamine
        pImpl->level *= std::max(0.1f, 1.0f + error);
    }
    NLM_LOG_INFO("Dopamine signaled prediction error: " + std::to_string(error));
}

void Dopamine::modulateMotivation(float motivation) {
    pImpl->motivation = std::max(0.0f, std::min(1.0f, motivation));
    // Motivation affects planning horizon
    pImpl->planningHorizon = pImpl->motivation * 5.0f; // Steps
    NLM_LOG_INFO("Dopamine modulated motivation to " + std::to_string(pImpl->motivation));
}

void Dopamine::enhanceWorkingMemory(float boost) {
    pImpl->workingMemoryBoost = std::max(pImpl->workingMemoryBoost, boost);
    NLM_LOG_INFO("Dopamine enhanced working memory by " + std::to_string(boost));
}

void Dopamine::scheduleFutureActions(float planningHorizon) {
    pImpl->planningHorizon = planningHorizon;
    NLM_LOG_INFO("Dopamine scheduled future actions with horizon: " + std::to_string(planningHorizon) + " steps");
}

} // namespace nlm