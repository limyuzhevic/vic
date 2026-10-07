#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityBaseline;
    float attentionBias;
    float memoryConsolidation;
    
    Impl() 
        : level(0.0f) 
        , baseline(0.0f) 
        , peak(1.0f) 
        , decayRate(0.1f) 
        , releaseRate(1.0f) 
        , plasticityBaseline(0.5f) 
        , attentionBias(0.0f) 
        , memoryConsolidation(0.0f) {}
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
    // Real dopamine-modulated plasticity factor:
    // - Low DA: High plasticity (early learning)
    // - Medium DA: Moderate plasticity (optimal learning)
    // - High DA: Low plasticity (performance stabilization)
    float plasticity = 0.5f + 0.5f * pImpl->level;
    
    // Dopamine also modulates attention to task-relevant stimuli
    float attentionMod = 1.0f + pImpl->attentionBias * pImpl->level;
    
    // And memory consolidation strength
    float memoryMod = 0.1f + pImpl->memoryConsolidation * pImpl->level;
    
    return plasticity * attentionMod * memoryMod;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics:
    // - Decay toward baseline over time
    // - Baseline changes with behavioral state
    // - Burst responses to rewards and prediction errors
    
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Dopamine::signalReward(float reward) {
    // Reward signaling with temporal dynamics:
    // - Phasic response scaled by reward prediction error
    // - Coreleased plasticity tracking
    // - Subsequent persistent activity
    
    // Phasic response: immediate dopamine burst
    float phasic = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasic);
    
    // Target baseline adjustment for future responses
    pImpl->baseline = std::max(pImpl->baseline, pImpl->level * 0.7f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling:
    // - Positive error (better than expected) → dopamine increase
    // - Negative error (worse than expected) → dopamine decrease
    // - Magnitude encodes surprise
    
    float delta = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + delta);
}

void Dopamine::setAttentionBias(float bias) {
    pImpl->attentionBias = std::clamp(bias, -0.5f, 0.5f);
}

void Dopamine::setMemoryConsolidation(float consolidation) {
    pImpl->memoryConsolidation = std::clamp(consolidation, 0.0f, 2.0f);
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    float attentionGain;
    float memoryScope;
    float corticalExcitation;
    
    Impl() 
        : level(0.0f) 
        , baseline(0.0f) 
        , decayRate(0.05f) 
        , attentionGain(1.0f) 
        , memoryScope(1.0f) 
        , corticalExcitation(0.0f) {}
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
    // Acetylcholine modulates attention and working memory:
    // - Increases neural excitability in cortex
    // - Enhances signal-to-noise ratio
    // - Reduces distractibility
    float attentionEffect = 1.0f + pImpl->attentionGain * pImpl->level;
    float workingMemoryEffect = 1.0f + pImpl->memoryScope * pImpl->level;
    
    return attentionEffect * workingMemoryEffect;
}

void Acetylcholine::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 2.0f);
}

void Acetylcholine::setMemoryScope(float scope) {
    pImpl->memoryScope = std::clamp(scope, 0.5f, 2.0f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float arousal;
    float locusCoeruleusGain;
    float vigilance;
    
    Impl() 
        : level(0.0f) 
        , baseline(0.0f) 
        , decayRate(0.08f) 
        , arousal(0.0f) 
        , locusCoeruleusGain(1.0f) 
        , vigilance(0.0f) {}
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
    // Norepinephrine modulates arousal and vigilance:
    // - Increases overall cortical excitability
    // - Enhances signal detection (neural noise reduction)
    // - Improves signal-to-noise ratio
    float arousalEffect = 1.0f + pImpl->arousal * pImpl->level;
    float vigilanceEffect = 1.0f + pImpl->vigilance * pImpl->level;
    
    return arousalEffect * vigilanceEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::setArousal(float arousal) {
    pImpl->arousal = std::clamp(arousal, 0.0f, 1.5f);
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = std::clamp(vigilance, 0.0f, 1.0f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    float mood;
    float impulsivity;
    float socialBehavior;
    float workingMemoryCost;
    
    Impl() 
        : level(0.0f) 
        , baseline(0.0f) 
        , decayRate(0.06f) 
        , mood(0.0f) 
        , impulsivity(0.0f) 
        , socialBehavior(0.0f) 
        , workingMemoryCost(0.0f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior:
    // - Improves working memory with moderate levels
    // - Reduces impulsivity and risk-taking
    // - Enhances pattern separation (less interference)
    float workingMemoryEffect = 1.0f - pImpl->workingMemoryCost * pImpl->level;
    float moodEffect = 1.0f + pImpl->mood * pImpl->level;
    
    return workingMemoryEffect * moodEffect;
}

void Serotonin::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::setMood(float mood) {
    pImpl->mood = std::clamp(mood, -0.5f, 0.5f);
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f);
}

void Serotonin::setSocialBehavior(float socialBehavior) {
    pImpl->socialBehavior = std::clamp(socialBehavior, 0.0f, 1.0f);
}

void Serotonin::setWorkingMemoryCost(float cost) {
    pImpl->workingMemoryCost = std::clamp(cost, 0.0f, 1.0f);
}

} // namespace nlm
