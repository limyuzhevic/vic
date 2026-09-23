#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention effects
    float attentionStrength;
    float memoryConsolidationRate;
    
    // Effects on working memory
    float workingMemoryBoost;
    float episodicMemoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , attentionStrength(1.0f), memoryConsolidationRate(1.0f)
        , workingMemoryBoost(1.0f), episodicMemoryBoost(1.0f) {}
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
    // ACh modulates attention and memory consolidation
    // Higher ACh improves information processing and memory strength
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float salience) {
    // ACh responds to salient stimuli, boosting attention and learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->releaseRate);
}

void Acetylcholine::boostMemoryConsolidation(float strength) {
    // Enhance memory consolidation process
    pImpl->memoryConsolidationRate = std::min(2.0f, 1.0f + strength);
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

void Acetylcholine::setAttentionStrength(float strength) {
    pImpl->attentionStrength = std::clamp(strength, 0.1f, 2.0f);
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

void Acetylcholine::setMemoryConsolidationRate(float rate) {
    pImpl->memoryConsolidationRate = std::clamp(rate, 0.5f, 3.0f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal effects
    float arousalLevel;
    float vigilanceLevel;
    float stressResponse;
    
    // Effects on neural excitability
    float excitabilityBoost;
    float reactionTimeEnhancement;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , arousalLevel(1.0f), vigilanceLevel(1.0f), stressResponse(0.0f)
        , excitabilityBoost(1.0f), reactionTimeEnhancement(1.0f) {}
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
    // NE modulates arousal and alertness
    // Higher NE increases neural excitability and plasticity
    return 0.5f + 0.5f * pImpl->level * pImpl->arousalLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity) {
    // NE responds to salient/stressful events, increasing arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
    
    // Stress response also affects other systems
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + intensity * 0.1f);
}

void Norepinephrine::setArousalLevel(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.5f, 2.0f);
}

void Norepinephrine::setVigilanceLevel(float level) {
    pImpl->vigilanceLevel = std::clamp(level, 0.5f, 2.0f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood effects
    float moodLevel;
    float impulsivity;
    float socialBehavior;
    
    // Effects on plasticity
    float plasticityModulation;
    float explorationBias;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , moodLevel(1.0f), impulsivity(1.0f), socialBehavior(1.0f)
        , plasticityModulation(1.0f), explorationBias(1.0f) {}
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
    // Serotonin modulates mood and social behavior
    // Level serotonin promotes stability and social learning
    return 0.5f + 0.5f * pImpl->level * pImpl->plasticityModulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float moodShift) {
    // Serotonin responds to social and mood contexts
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodShift * pImpl->releaseRate);
    
    // Promotes social behavior and reduces impulsivity
    pImpl->socialBehavior = std::min(2.0f, pImpl->socialBehavior + moodShift * 0.2f);
    pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity - moodShift * 0.1f);
}

void Serotonin::setMoodLevel(float level) {
    pImpl->moodLevel = std::clamp(level, 0.5f, 2.0f);
}

void Serotonin::setImpulsivity(float level) {
    pImpl->impulsivity = std::clamp(level, 0.1f, 2.0f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

} // namespace nlm
