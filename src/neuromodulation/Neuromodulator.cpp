#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidationFactor;
    float attentionBias;
    float learningRateModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             memoryConsolidationFactor(1.0f), attentionBias(0.0f), learningRateModulation(1.0f) {}
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
    // Enhanced: Higher dopamine increases plasticity, with memory modulation
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryConsolidationFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay memory consolidation factor
    pImpl->memoryConsolidationFactor = std::max(0.5f, pImpl->memoryConsolidationFactor - 0.01f * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Enhance memory consolidation on reward
    pImpl->memoryConsolidationFactor = std::min(2.0f, pImpl->memoryConsolidationFactor + 0.5f * reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error with learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Prediction error modulates learning rate
    pImpl->learningRateModulation = 1.0f + error * 0.5f;
}

void Dopamine::modulateMemoryConsolidation(bool strengthen) {
    if (strengthen) {
        pImpl->memoryConsolidationFactor = std::min(2.0f, pImpl->memoryConsolidationFactor + 0.2f);
    } else {
        pImpl->memoryConsolidationFactor = std::max(0.5f, pImpl->memoryConsolidationFactor - 0.1f);
    }
}

void Dopamine::modulateAttention(float bias) {
    pImpl->attentionBias = std::clamp(bias, -1.0f, 1.0f);
}

void Dopamine::modulateLearningRate(float rate) {
    pImpl->learningRateModulation = std::clamp(rate, 0.1f, 2.0f);
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // Memory binding strength
    float memoryBindingStrength;
    
    // Feature binding state
    std::vector<NeuronId> boundFeatures;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), memoryBindingStrength(1.0f) {}
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
    // ACh enhances plasticity for attention and memory
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay memory binding strength
    pImpl->memoryBindingStrength = std::max(0.5f, pImpl->memoryBindingStrength - 0.005f * static_cast<float>(dt));
}

void Acetylcholine::enhanceMemoryConsolidation(float strength) {
    pImpl->memoryBindingStrength = std::min(2.0f, pImpl->memoryBindingStrength + strength);
}

void Acetylcholine::increaseAttentionFocus(float focus) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + focus);
}

void Acetylcholine::bindFeatures(const std::vector<NeuronId>& neurons) {
    pImpl->boundFeatures = neurons;
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousal;
    float vigilance;
    
    // Stress accumulator
    float stress;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), arousal(0.5f), vigilance(0.5f), stress(0.0f) {}
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
    // NE enhances plasticity for learning under arousal
    return 1.0f + 0.3f * pImpl->arousal + 0.2f * pImpl->vigilance;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance
    pImpl->arousal = std::max(0.1f, pImpl->arousal - 0.01f * static_cast<float>(dt));
    pImpl->vigilance = std::max(0.1f, pImpl->vigilance - 0.02f * static_cast<float>(dt));
    
    // Decay stress
    pImpl->stress = std::max(0.0f, pImpl->stress - 0.005f * static_cast<float>(dt));
}

void Norepinephrine::setArousalLevel(float level) {
    pImpl->arousal = std::clamp(level, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + level * 0.5f);
}

void Norepinephrine::increaseVigilance(float amount) {
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.3f);
}

void Norepinephrine::activateStressResponse(float intensity) {
    pImpl->stress = std::min(1.0f, pImpl->stress + intensity);
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * 0.8f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // Mood level
    float mood;
    
    // Impulsivity control
    float impulseControl;
    
    // Social engagement
    float socialEngagement;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), mood(0.5f), impulseControl(0.5f), socialEngagement(0.5f) {}
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
    // Serotonin modulates plasticity for emotional learning
    return 1.0f - 0.2f * std::abs(pImpl->mood - 0.5f) + 0.1f * pImpl->socialEngagement;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood, impulse control, and social engagement
    pImpl->mood = std::max(0.1f, pImpl->mood - 0.008f * static_cast<float>(dt));
    pImpl->impulseControl = std::max(0.1f, pImpl->impulseControl - 0.006f * static_cast<float>(dt));
    pImpl->socialEngagement = std::max(0.1f, pImpl->socialEngagement - 0.007f * static_cast<float>(dt));
}

void Serotonin::regulateMood(float moodLevel) {
    pImpl->mood = std::clamp(moodLevel, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * 0.4f);
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.3f);
}

void Serotonin::modulateSocialBehavior(float socialness) {
    pImpl->socialEngagement = std::clamp(socialness, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialness * 0.5f);
}

} // namespace nlm
