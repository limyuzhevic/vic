#include "Neuromodulator.hpp"
#include <algorithm>

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
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation parameters
    float attentionGain;
    float memoryEncodingStrength;
    
    // Memory parameters
    float workingMemoryBoost;
    float episodicMemoryConsolidation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionGain(1.0f), memoryEncodingStrength(0.8f),
             workingMemoryBoost(0.5f), episodicMemoryConsolidation(0.3f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACH"; // To be implemented
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances LTP, reduces LTD
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryEncodingStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention() {
    // Boost attention gain
    pImpl->attentionGain = std::min(2.0f, pImpl->attentionGain + pImpl->level * 0.5f);
}

void Acetylcholine::boostWorkingMemory(float duration) {
    // Enhance working memory trace strength
    pImpl->workingMemoryBoost = std::min(1.5f, pImpl->workingMemoryBoost + pImpl->level * 0.3f);
}

void Acetylcholine::enhanceMemoryEncoding() {
    // Improve memory encoding efficiency
    pImpl->memoryEncodingStrength = std::min(1.2f, pImpl->memoryEncodingStrength + pImpl->level * 0.4f);
}

void Acetylcholine::promoteEpisodicConsolidation() {
    // Strengthen episodic memory consolidation
    pImpl->episodicMemoryConsolidation = std::min(1.0f, pImpl->episodicMemoryConsolidation + pImpl->level * 0.2f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance parameters
    float arousalLevel;
    float vigilance;
    float learningRate;
    float selectiveAttention;
    
    // Memory parameters
    float memoryConsolidation;
    float memoryReconsolidation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.2f),
             arousalLevel(1.0f), vigilance(0.5f), learningRate(1.0f), selectiveAttention(0.8f),
             memoryConsolidation(0.6f), memoryReconsolidation(0.4f) {}
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
    // NE enhances plasticity, modulates learning rates
    return 0.3f + 0.7f * pImpl->level * pImpl->learningRate;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal() {
    // Boost arousal for vigilance
    pImpl->arousalLevel = std::min(2.0f, pImpl->arousalLevel + pImpl->level * 0.3f);
}

void Norepinephrine::enhanceVigilance() {
    // Improve selective attention and vigilance
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + pImpl->level * 0.4f);
}

void Norepinephrine::modulateLearningRate(float factor) {
    // Modulate learning rate based on NE level
    pImpl->learningRate = std::clamp(0.5f, pImpl->learningRate + factor * pImpl->level, 2.0f);
}

void Norepinephrine::enhanceSelectiveAttention() {
    // Boost selective attention for relevant stimuli
    pImpl->selectiveAttention = std::min(1.0f, pImpl->selectiveAttention + pImpl->level * 0.3f);
}

void Norepinephrine::promoteMemoryConsolidation() {
    // Enhance memory consolidation
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + pImpl->level * 0.3f);
}

void Norepinephrine::enhanceMemoryReconsolidation() {
    // Enhance memory reconsolidation during recall
    pImpl->memoryReconsolidation = std::min(1.0f, pImpl->memoryReconsolidation + pImpl->level * 0.2f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and behavioral flexibility parameters
    float mood;
    float behavioralFlexibility;
    float rewardProcessing;
    float impulseControl;
    
    // Memory parameters
    float retrievalStrength;
    float patternSeparation;
    float patternCompletion;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             mood(0.5f), behavioralFlexibility(0.7f), rewardProcessing(0.8f), impulseControl(0.5f),
             retrievalStrength(0.6f), patternSeparation(0.7f), patternCompletion(0.6f) {}
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
    // 5-HT regulates plasticity, reduces runaway excitation
    return 0.8f - 0.3f * pImpl->level * pImpl->impulseControl;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::regulateMood() {
    // Modulate overall mood
    pImpl->mood = std::clamp(0.0f, pImpl->mood + pImpl->level * 0.2f, 1.0f);
}

void Serotonin::enhanceBehavioralFlexibility() {
    // Improve ability to adapt to new situations
    pImpl->behavioralFlexibility = std::min(1.0f, pImpl->behavioralFlexibility + pImpl->level * 0.3f);
}

void Serotonin::enhanceRewardProcessing() {
    // Strengthen reward processing and prediction
    pImpl->rewardProcessing = std::min(1.0f, pImpl->rewardProcessing + pImpl->level * 0.2f);
}

void Serotonin::improveImpulseControl() {
    // Enhance impulse control and inhibition
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + pImpl->level * 0.4f);
}

void Serotonin::enhanceMemoryRetrieval() {
    // Boost memory retrieval strength
    pImpl->retrievalStrength = std::min(1.0f, pImpl->retrievalStrength + pImpl->level * 0.3f);
}

void Serotonin::promotePatternSeparation() {
    // Enhance ability to distinguish similar memories
    pImpl->patternSeparation = std::min(1.0f, pImpl->patternSeparation + pImpl->level * 0.3f);
}

void Serotonin::promotePatternCompletion() {
    // Enhance ability to complete incomplete patterns
    pImpl->patternCompletion = std::min(1.0f, pImpl->patternCompletion + pImpl->level * 0.2f);
}

} // namespace nlm
