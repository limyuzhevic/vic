#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
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

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionLevel;
    float memoryStability;
    float baseline;
    float decayRate;
    float attentionBoostRate;
    float memoryConsolidationRate;
    float workingMemoryStrength;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , attentionLevel(0.5f)
        , memoryStability(0.5f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , attentionBoostRate(0.5f)
        , memoryConsolidationRate(0.3f)
        , workingMemoryStrength(0.5f) {}
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
    // ACh modulates working memory plasticity
    // Higher ACh increases synaptic plasticity for memory-related circuits
    return 0.3f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth attention and memory stability based on level
    pImpl->attentionLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->memoryStability = 0.2f + 0.8f * pImpl->level;
}

void Acetylcholine::enhanceAttention(float focusWeight) {
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + focusWeight * pImpl->attentionBoostRate);
}

void Acetylcholine::modulateWorkingMemory(float memoryStrength) {
    pImpl->workingMemoryStrength = std::clamp(memoryStrength, 0.0f, 1.0f);
    // Boost current level based on memory strength
    pImpl->level = std::min(1.0f, pImpl->level + memoryStrength * 0.2f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::consolidateMemory(float memoryStability) {
    pImpl->memoryStability = std::clamp(memoryStability, 0.0f, 1.0f);
    // Enhance current level for memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + memoryStability * 0.3f);
}

void Acetylcholine::enhanceLongTermMemory(float consolidationFactor) {
    pImpl->level = std::min(1.0f, pImpl->level + consolidationFactor * pImpl->memoryConsolidationRate);
}

float Acetylcholine::getMemoryStability() const {
    return pImpl->memoryStability;
}

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float vigilanceLevel;
    float baseline;
    float decayRate;
    float arousalBoostRate;
    float vigilanceBoostRate;
    float neuronExcitability;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , arousalLevel(0.5f)
        , vigilanceLevel(0.5f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , arousalBoostRate(0.5f)
        , vigilanceBoostRate(0.5f)
        , neuronExcitability(1.0f) {}
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
    // NE modulates arousal-related plasticity
    // Higher NE increases plasticity for arousal-related circuits
    return 0.2f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth arousal and vigilance based on level
    pImpl->arousalLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->vigilanceLevel = 0.4f + 0.6f * pImpl->level;
    
    // NE increases neuron excitability
    pImpl->neuronExcitability = 1.0f + 0.8f * pImpl->level;
}

void Norepinephrine::increaseArousal(float intensity) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + intensity * pImpl->arousalBoostRate);
    pImpl->level = std::min(1.0f, pImpl->level + intensity * 0.2f);
}

void Norepinephrine::regulateVigilance(float alertness) {
    pImpl->vigilanceLevel = std::clamp(alertness, 0.0f, 1.0f);
    pImpl->level = std::min(1.0f, pImpl->level + alertness * 0.3f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorLevel;
    float baseline;
    float decayRate;
    float moodBoostRate;
    float impulseControlRate;
    float sociabilityRate;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , moodLevel(0.5f)
        , impulsivityLevel(0.5f)
        , socialBehaviorLevel(0.5f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , moodBoostRate(0.5f)
        , impulseControlRate(0.5f)
        , sociabilityRate(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth mood and related states based on level
    pImpl->moodLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->impulsivityLevel = 0.7f - 0.5f * pImpl->level; // Inverse: higher serotonin reduces impulsivity
    pImpl->socialBehaviorLevel = 0.4f + 0.6f * pImpl->level;
}

void Serotonin::enhanceMood(float positivity) {
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + positivity * pImpl->moodBoostRate);
    pImpl->level = std::min(1.0f, pImpl->level + positivity * 0.2f);
}

void Serotonin::reduceImpulsivity(float control) {
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - control * pImpl->impulseControlRate);
    pImpl->level = std::min(1.0f, pImpl->level + control * 0.15f);
}

void Serotonin::modulateSocialBehavior(float sociability) {
    pImpl->socialBehaviorLevel = std::clamp(sociability, 0.0f, 1.0f);
    pImpl->level = std::min(1.0f, pImpl->level + sociability * 0.25f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getSocialBehaviorLevel() const {
    return pImpl->socialBehaviorLevel;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->socialBehaviorLevel = 0.5f;
}

} // namespace nlm