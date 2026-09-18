#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace nlm {

// ============ DOPAMINE IMPLEMENTATION ============

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

// ============ ACETYLCHOLINE IMPLEMENTATION =============

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityBoost;
    float attentionModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.05f)  // Baseline level
        , baseline(0.05f)
        , peak(0.3f)
        , decayRate(0.02f)
        , releaseRate(0.5f)
        , plasticityBoost(1.5f)
        , attentionModulation(1.2f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized for attention and memory");
}

void Acetylcholine::boostMemoryEncoding(float amount) {
    // Store for use in memory systems
    if (pImpl->brain) {
        // TODO: Integrate with memory systems
        // For now, just log the effect
        NLM_LOG_INFO(std::string("Acetylcholine boosting memory encoding by ") + std::to_string(amount));
    }
}

void Acetylcholine::enhanceAttention(float amount) {
    pImpl->attentionModulation = 1.0f + amount;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for learning and memory
    // Higher ACh levels increase plasticity
    float factor = 1.0f + pImpl->plasticityBoost * pImpl->level;
    return std::clamp(factor, 0.5f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: baseline with task-dependent bursts
    float targetLevel = pImpl->baseline;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise
    static float noise = 0.0f;
    noise = noise * 0.95f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
    pImpl->level += noise * 0.1f;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionModulation = 1.2f;
}

// ============ NORADRENALINE IMPLEMENTATION ===========

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilance;
    float locusCoeruleusGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.05f)  // Baseline arousal
        , baseline(0.05f)
        , peak(0.4f)
        , decayRate(0.03f)
        , releaseRate(0.8f)
        , arousalLevel(1.0f)
        , vigilance(0.8f)
        , locusCoeruleusGain(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized for arousal and vigilance");
}

void Norepinephrine::signalStressOrNovelty(float input) {
    // LC neurons respond to novel, surprising, or stress-inducing stimuli
    if (input > 0.0f) {
        float burst = std::min(0.15f, input * pImpl->releaseRate);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Enhance vigilance
        enhanceVigilance(input * 0.3f);
        
        // Increase arousal
        increaseArousal(input * 0.2f);
    }
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->arousalLevel = 1.0f + amount;
}

void Norepinephrine::enhanceVigilance(float amount) {
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + amount);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for focus and learning under stress
    // Inverted U-shaped relationship: optimal NE for learning
    float optimal = 0.2f;
    float distance = std::abs(pImpl->level - optimal);
    float modulation = std::exp(-distance * 5.0f);
    
    float factor = 0.8f + modulation * 1.5f;  // Range: ~0.8 to 2.3
    return std::clamp(factor, 0.5f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: tonic and phasic signaling
    float targetLevel = pImpl->baseline;
    
    // Tonic decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise for phasic variations
    static float noise = 0.0f;
    noise = noise * 0.98f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
    pImpl->level += noise * 0.05f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 1.0f;
    pImpl->vigilance = 0.8f;
}

// ============ SEROTONIN IMPLEMENTATION ============

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float socialBehavior;
    
    Impl() 
        : brain(nullptr)
        , level(0.05f)  // Baseline serotonin
        , baseline(0.05f)
        , peak(0.25f)
        , decayRate(0.04f)
        , releaseRate(0.6f)
        , mood(0.8f)
        , impulsivity(1.0f)
        , socialBehavior(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized for mood and social behavior");
}

void Serotonin::signalRewardOrSocialFeedback(float feedback) {
    // 5-HT responds to reward prediction errors and social feedback
    if (feedback > 0.0f) {
        // Positive feedback (reward, acceptance) increases 5-HT
        float increase = std::min(0.08f, feedback * pImpl->releaseRate);
        pImpl->level = std::min(pImpl->peak, pImpl->level + increase);
        
        // Improve mood
        enhanceMood(feedback * 0.2f);
        
        // Enhance social behavior
        enhanceSocialBehavior(feedback * 0.1f);
    } else if (feedback < 0.0f) {
        // Negative feedback (punishment, rejection) decreases 5-HT
        float decrease = std::min(0.05f, std::abs(feedback) * pImpl->decayRate * 2.0f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease);
        
        // Increase impulsivity (negative mood)
        reduceImpulsivity(std::abs(feedback) * 0.2f);
    }
}

void Serotonin::enhanceMood(float amount) {
    pImpl->mood = std::min(1.0f, pImpl->mood + amount);
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity - amount);
}

void Serotonin::enhanceSocialBehavior(float amount) {
    pImpl->socialBehavior = std::min(1.5f, pImpl->socialBehavior + amount);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for behavioral inhibition and mood regulation
    // Higher 5-HT promotes stable, conservative learning
    float factor = 0.7f + pImpl->level * 0.8f;  // Range: ~0.7 to 1.5
    return std::clamp(factor, 0.3f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics: slow, sustained modulation
    float targetLevel = pImpl->baseline;
    
    // Decay towards baseline (slow)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise
    static float noise = 0.0f;
    noise = noise * 0.95f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.02f;
    pImpl->level += noise * 0.05f;
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->mood = 0.8f;
    pImpl->impulsivity = 1.0f;
    pImpl->socialBehavior = 1.0f;
}

} // namespace nlm