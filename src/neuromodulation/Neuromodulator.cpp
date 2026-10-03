#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// ========== ACETYLCHOLINE ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionDrive;
    float memoryEnhancement;
    float arousalModulation;
    float encodingStrength;
    float focusLevel;
    float lastAttentionSignal;
    float lastEncodingSignal;
    float lastFocusSignal;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f),
             attentionDrive(1.0f), memoryEnhancement(1.0f), arousalModulation(0.5f),
             encodingStrength(1.0f), focusLevel(0.5f), lastAttentionSignal(0.0f),
             lastEncodingSignal(0.0f), lastFocusSignal(0.0f) {}
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
    // ACh modulates plasticity by enhancing synaptic strengthening
    return 0.3f + 0.7f * pImpl->level;
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    // ACh enhances memory consolidation and encoding
    return 0.2f + 0.8f * pImpl->level;
}

float Acetylcholine::getAttentionModulationFactor() const {
    // ACh is the primary attention modulator
    return 0.1f + 0.9f * pImpl->level;
}

float Acetylcholine::getArousalModulationFactor() const {
    // ACh has moderate arousal effects
    return 0.3f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slowly decay signals
    pImpl->lastAttentionSignal *= (1.0f - pImpl->decayRate);
    pImpl->lastEncodingSignal *= (1.0f - pImpl->decayRate);
    pImpl->lastFocusSignal *= (1.0f - pImpl->decayRate);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->lastAttentionSignal = 0.0f;
    pImpl->lastEncodingSignal = 0.0f;
    pImpl->lastFocusSignal = 0.0f;
    pImpl->focusLevel = 0.5f;
}

void Acetylcholine::signalAttentionDrive(float strength) {
    // Strong attentional drive increases ACh release
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate);
    pImpl->lastAttentionSignal = strength;
    
    // Update focus level based on attention strength
    pImpl->focusLevel = std::min(1.0f, pImpl->focusLevel + strength * 0.1f);
}

void Acetylcholine::enhanceEncoding(float novelty) {
    // Novelty detection enhances ACh release for memory encoding
    if (novelty > 0.5f) {
        float boost = (novelty - 0.5f) * 2.0f * pImpl->attentionDrive;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    }
    pImpl->lastEncodingSignal = novelty * pImpl->encodingStrength;
}

void Acetylcholine::adjustFocus(float focusLevel) {
    // Adjust attentional focus level
    pImpl->focusLevel = std::clamp(focusLevel, 0.0f, 1.0f);
    pImpl->lastFocusSignal = std::abs(focusLevel - 0.5f) * 2.0f;
    
    // Focus level affects overall ACh level
    pImpl->level = pImpl->baseline + pImpl->focusLevel * 0.3f;
}

// ========== NORADRENALINE ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalIncrease;
    float sensoryGain;
    float vigilanceLevel;
    float locusCoeruleusActivity;
    float lastArousalSignal;
    float lastSensorySignal;
    float lastVigilanceSignal;
    
    Impl() : level(0.05f), baseline(0.02f), peak(1.0f), decayRate(0.08f), releaseRate(2.5f),
             arousalIncrease(1.0f), sensoryGain(1.0f), vigilanceLevel(0.5f),
             locusCoeruleusActivity(0.0f), lastArousalSignal(0.0f),
             lastSensorySignal(0.0f), lastVigilanceSignal(0.0f) {}
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
    // NE has complex effects on plasticity (phasic/tonic)
    float tonicEffect = 0.2f + 0.3f * pImpl->level;
    float phasicEffect = pImpl->lastArousalSignal * 0.5f;
    return tonicEffect + phasicEffect;
}

float Norepinephrine::getMemoryConsolidationFactor() const {
    // NE enhances memory consolidation during arousal
    return 0.3f + 0.6f * pImpl->level + pImpl->lastSensorySignal * 0.2f;
}

float Norepinephrine::getAttentionModulationFactor() const {
    // NE modulates alertness and attention
    float alertnessEffect = 0.2f + 0.5f * pImpl->level;
    float vigilanceEffect = pImpl->lastVigilanceSignal * 0.3f;
    return alertnessEffect + vigilanceEffect;
}

float Norepinephrine::getArousalModulationFactor() const {
    // NE is the primary arousal modulator
    float tonicArousal = pImpl->level * 0.8f;
    float phasicArousal = pImpl->lastArousalSignal * 1.2f;
    return tonicArousal + phasicArousal;
}

void Norepinephrine::update(TimestepDuration dt) {
    // More complex dynamics with two time constants
    float slowDecay = pImpl->decayRate * 0.5f;
    float fastDecay = pImpl->decayRate * 2.0f;
    
    // Decay with both fast and slow components
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - slowDecay * static_cast<float>(dt));
    
    // Fast decay of phasic signals
    pImpl->lastArousalSignal *= (1.0f - fastDecay);
    pImpl->lastSensorySignal *= (1.0f - fastDecay);
    pImpl->lastVigilanceSignal *= (1.0f - fastDecay);
    
    // Update locus coeruleus activity based on level
    pImpl->locusCoeruleusActivity = pImpl->level * 0.8f + 0.1f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->lastArousalSignal = 0.0f;
    pImpl->lastSensorySignal = 0.0f;
    pImpl->lastVigilanceSignal = 0.0f;
    pImpl->vigilanceLevel = 0.5f;
}

void Norepinephrine::signalArousalIncrease(float amount) {
    // Arousal signals increase NE release
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
    pImpl->lastArousalSignal = amount;
    
    // Increase vigilance
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + amount * 0.1f);
}

void Norepinephrine::enhanceSensoryGain(float gain) {
    // Sensory stimuli increase NE release (sensory gain modulation)
    pImpl->level = std::min(pImpl->peak, pImpl->level + gain * pImpl->releaseRate * 0.5f);
    pImpl->lastSensorySignal = gain;
    
    // Increase sensory processing efficiency
    pImpl->sensoryGain = std::min(2.0f, pImpl->sensoryGain + gain * 0.5f);
}

void Norepinephrine::adjustVigilance(float level) {
    // Adjust vigilance level
    pImpl->vigilanceLevel = std::clamp(level, 0.0f, 1.0f);
    pImpl->lastVigilanceSignal = std::abs(level - 0.5f) * 2.0f;
    
    // Vigilance affects NE level
    pImpl->level = pImpl->baseline + pImpl->vigilanceLevel * 0.3f;
}

// ========== SEROTONIN ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivityLevel;
    float socialLearningBoost;
    float rapheNucleusActivity;
    float lastMoodSignal;
    float lastImpulsivitySignal;
    float lastSocialSignal;
    
    Impl() : level(0.05f), baseline(0.03f), peak(1.0f), decayRate(0.1f), releaseRate(2.0f),
             moodLevel(0.5f), impulsivityLevel(0.5f), socialLearningBoost(1.0f),
             rapheNucleusActivity(0.0f), lastMoodSignal(0.0f),
             lastImpulsivitySignal(0.0f), lastSocialSignal(0.0f) {}
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
    // Serotonin modulates plasticity in complex ways (inhibits impulsive learning)
    float baselinePlasticity = 0.8f;
    float impulseInhibition = pImpl->impulsivityLevel * 0.3f;
    float moodModulation = pImpl->lastMoodSignal * 0.2f;
    return baselinePlasticity - impulseInhibition + moodModulation;
}

float Serotonin::getMemoryConsolidationFactor() const {
    // Serotonin enhances memory consolidation, especially for emotional events
    return 0.3f + 0.5f * pImpl->level + pImpl->lastMoodSignal * 0.3f;
}

float Serotonin::getAttentionModulationFactor() const {
    // Serotonin modulates attention and impulse control
    float impulseControl = 0.8f - pImpl->impulsivityLevel * 0.6f;
    float moodEffect = pImpl->lastMoodSignal * 0.2f;
    return impulseControl + moodEffect;
}

float Serotonin::getArousalModulationFactor() const {
    // Serotonin has mild arousal effects
    float tonicArousal = pImpl->level * 0.4f;
    float moodArousal = pImpl->lastMoodSignal * 0.3f;
    return tonicArousal + moodArousal;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin has slower dynamics than DA/NE
    float slowDecay = pImpl->decayRate * 1.5f;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - slowDecay * static_cast<float>(dt));
    
    // Decay phasic signals
    pImpl->lastMoodSignal *= (1.0f - slowDecay);
    pImpl->lastImpulsivitySignal *= (1.0f - slowDecay);
    
    // Update raphe nucleus activity
    pImpl->rapheNucleusActivity = pImpl->level * 0.7f + 0.1f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->lastMoodSignal = 0.0f;
    pImpl->lastImpulsivitySignal = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
}

void Serotonin::signalMoodState(float moodLevel) {
    // Mood states affect serotonin release
    pImpl->moodLevel = std::clamp(moodLevel, 0.0f, 1.0f);
    
    if (moodLevel > 0.7f) {
        // High positive mood increases serotonin
        float boost = (moodLevel - 0.7f) * 3.0f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    } else if (moodLevel < 0.3f) {
        // Low mood decreases serotonin
        float decrease = (0.3f - moodLevel) * 2.0f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease * pImpl->releaseRate * 0.5f);
    }
    
    pImpl->lastMoodSignal = std::abs(moodLevel - 0.5f) * 2.0f;
}

void Serotonin::adjustImpulsivity(float level) {
    // Adjust impulsivity level
    pImpl->impulsivityLevel = std::clamp(level, 0.0f, 1.0f);
    pImpl->lastImpulsivitySignal = std::abs(level - 0.5f) * 2.0f;
    
    // Impulsivity affects serotonin level
    pImpl->level = pImpl->baseline + (1.0f - pImpl->impulsivityLevel) * 0.2f;
}

void Serotonin::facilitateSocialLearning(float amount) {
    // Social contexts enhance serotonin release
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate * 0.8f);
    pImpl->lastSocialSignal = amount;
    
    // Boost social learning
    pImpl->socialLearningBoost = std::min(2.0f, pImpl->socialLearningBoost + amount * 0.5f);
}

// ========== DOPAMINE ==========

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float tonicLevel;
    float phasicLevel;
    float rewardPredictionError;
    float reinforcementStrength;
    float learningRate;
    float burstFrequency;
    float lastRewardSignal;
    float lastErrorSignal;
    
    Impl() : level(0.1f), baseline(0.05f), peak(1.0f), decayRate(0.15f), releaseRate(2.0f),
             tonicLevel(0.1f), phasicLevel(0.0f), rewardPredictionError(0.0f),
             reinforcementStrength(1.0f), learningRate(0.01f), burstFrequency(0.0f),
             lastRewardSignal(0.0f), lastErrorSignal(0.0f) {}
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
    // Dopamine modulates plasticity in a complex, nonlinear way
    float tonicEffect = pImpl->tonicLevel * 0.3f;
    float phasicEffect = pImpl->phasicLevel * 0.7f;
    float rewardEffect = pImpl->lastRewardSignal * 0.5f;
    float errorEffect = pImpl->rewardPredictionError * 0.4f;
    
    return tonicEffect + phasicEffect + rewardEffect + errorEffect;
}

float Dopamine::getMemoryConsolidationFactor() const {
    // Dopamine enhances reward-based memory consolidation
    float rewardConsolidation = pImpl->lastRewardSignal * pImpl->reinforcementStrength;
    float errorConsolidation = pImpl->rewardPredictionError * 0.5f;
    return 0.2f + rewardConsolidation + errorConsolidation;
}

float Dopamine::getAttentionModulationFactor() const {
    // Dopamine modulates attention and motivation
    float motivationEffect = pImpl->phasicLevel * 0.8f;
    float rewardEffect = pImpl->lastRewardSignal * 0.3f;
    return motivationEffect + rewardEffect;
}

float Dopamine::getArousalModulationFactor() const {
    // Dopamine has moderate arousal effects
    float tonicArousal = pImpl->tonicLevel * 0.5f;
    float phasicArousal = pImpl->phasicLevel * 1.0f;
    return tonicArousal + phasicArousal;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine dynamics with both tonic and phasic components
    float tonicDecay = pImpl->decayRate * 0.5f;
    float phasicDecay = pImpl->decayRate * 3.0f;
    
    // Tonic component slowly decays
    pImpl->tonicLevel = std::max(pImpl->baseline, 
                                pImpl->tonicLevel - tonicDecay * static_cast<float>(dt));
    
    // Phasic component decays faster
    pImpl->phasicLevel *= (1.0f - phasicDecay);
    
    // Overall level is combination of tonic and phasic
    float targetLevel = pImpl->tonicLevel + pImpl->phasicLevel;
    pImpl->level = targetLevel;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->tonicLevel = pImpl->baseline;
    pImpl->phasicLevel = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->lastRewardSignal = 0.0f;
    pImpl->lastErrorSignal = 0.0f;
    pImpl->burstFrequency = 0.0f;
}

void Dopamine::signalReward(float reward) {
    // Reward signals cause phasic dopamine bursts
    pImpl->phasicLevel = std::min(1.0f, reward * pImpl->releaseRate);
    pImpl->lastRewardSignal = reward;
    pImpl->rewardPredictionError += (reward - pImpl->rewardPredictionError) * 0.1f;
    
    // Enhance reinforcement pathways
    pImpl->reinforcementStrength = std::min(2.0f, pImpl->reinforcementStrength + reward * 0.5f);
    
    // Update learning rate based on reward
    pImpl->learningRate = std::min(0.1f, pImpl->learningRate + reward * 0.02f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction errors modulate dopamine
    pImpl->rewardPredictionError = error;
    pImpl->lastErrorSignal = error;
    
    if (error > 0.1f) {
        // Positive prediction error increases phasic dopamine
        pImpl->phasicLevel = std::min(1.0f, pImpl->phasicLevel + error * pImpl->releaseRate * 0.5f);
    } else if (error < -0.1f) {
        // Negative prediction error decreases dopamine
        pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel - std::abs(error) * pImpl->releaseRate * 0.3f);
    }
}

void Dopamine::strengthenRewardPathways(float strength) {
    // Reward-based learning strengthens synaptic connections
    pImpl->reinforcementStrength = std::min(3.0f, pImpl->reinforcementStrength + strength * 0.5f);
    pImpl->burstFrequency = std::min(10.0f, pImpl->burstFrequency + strength * 0.2f);
}

void Dopamine::adjustMotivation(float level) {
    // Motivation affects tonic dopamine level
    pImpl->tonicLevel = std::clamp(level, 0.0f, 1.0f) * 0.5f + pImpl->baseline;
    pImpl->phasicLevel = level * 0.5f;  // Phasic component tracks motivation
}

} // namespace nlm

