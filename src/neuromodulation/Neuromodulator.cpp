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
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm

struct Acetylcholine::Impl {
    float level;
    float attentionMod;
    float memoryMod;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), attentionMod(1.0f), memoryMod(1.0f), decayRate(0.05f), releaseRate(1.0f) {}
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
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention() {
    // Enhance selective attention
    pImpl->attentionMod = 1.0f + pImpl->level * 2.0f;
}

void Acetylcholine::enhanceMemoryEncoding() {
    // Enhance memory encoding
    pImpl->memoryMod = 1.0f + pImpl->level * 1.5f;
}

struct Norepinephrine::Impl {
    float level;
    float arousalMod;
    float learningMod;
    float detectionMod;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), arousalMod(1.0f), learningMod(1.0f), detectionMod(1.0f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // NE enhances arousal and learning rate
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::enhanceArousal() {
    // Enhance arousal and vigilance
    pImpl->arousalMod = 1.0f + pImpl->level * 1.5f;
}

void Norepinephrine::enhanceLearningRate() {
    // Enhance learning rate
    pImpl->learningMod = 1.0f + pImpl->level * 2.0f;
}

void Norepinephrine::enhanceSignalDetection() {
    // Enhance signal detection
    pImpl->detectionMod = 1.0f + pImpl->level * 1.8f;
}

struct Serotonin::Impl {
    float level;
    float moodMod;
    float flexibilityMod;
    float explorationMod;
    float developmentMod;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), moodMod(1.0f), flexibilityMod(1.0f), explorationMod(1.0f), developmentMod(1.0f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // 5-HT modulates mood and behavioral flexibility
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::enhanceMood() {
    // Enhance mood
    pImpl->moodMod = 1.0f + pImpl->level * 0.8f;
}

void Serotonin::enhanceFlexibility() {
    // Enhance behavioral flexibility
    pImpl->flexibilityMod = 1.0f + pImpl->level * 1.2f;
}

void Serotonin::enhanceExploration() {
    // Enhance exploration vs exploitation balance
    pImpl->explorationMod = 1.0f - pImpl->level * 0.5f;  // Higher 5-HT favors exploration
}

void Serotonin::enhanceDevelopment() {
    // Enhance development processes
    pImpl->developmentMod = 1.0f + pImpl->level * 1.3f;
}

} // namespace nlm
