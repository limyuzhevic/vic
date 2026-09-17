#include "Neuromodulator.hpp"
#include "../brain/Neuron.hpp"
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

NeuromodulatorType Dopamine::getType() const {
    return NeuromodulatorType::Dopamine;
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: Higher dopamine enhances LTP, suppresses LTD
    // This is a simplified model based on biological evidence
    if (pImpl->level < 0.1f) {
        // Low dopamine: suppress plasticity
        return 0.2f;
    } else if (pImpl->level < 0.5f) {
        // Medium dopamine: moderate plasticity
        return 0.5f + pImpl->level;
    } else {
        // High dopamine: enhance plasticity, favor LTP
        return 0.8f + pImpl->level * 0.2f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: exponential decay with activity-dependent replenishment
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Dopamine::applyToNeuron(Neuron* neuron, float factor) {
    if (!neuron) return;
    
    // Dopamine modulates neuron excitability through D1/D2 receptors
    // Positive dopamine increases excitability (lowers effective threshold)
    // Negative dopamine decreases excitability (increases effective threshold)
    float modulation = factor * (pImpl->level - 0.5f); // Center around 0.5
    
    if (modulation > 0.0f) {
        // Dopamine excites neuron
        neuron->injectCurrent(modulation * 2.0f);
    } else {
        // Dopamine inhibits neuron
        neuron->injectCurrent(modulation * 0.5f);
    }
}

void Dopamine::applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) {
    // Real dopamine modulation of plasticity rules
    float plasticityFactor = getPlasticityFactor();
    
    // Scale LTP and LTD based on dopamine level
    ltpWeight *= plasticityFactor;
    ltdWeight /= plasticityFactor;  // Inverse relationship
    
    // Ensure reasonable bounds
    ltpWeight = std::clamp(ltpWeight, 0.001f, 0.1f);
    ltdWeight = std::clamp(ltdWeight, 0.001f, 0.15f);
}

void Dopamine::signalReward(float reward) {
    // Real reward prediction error signal
    // Dopamine neurons burst for unexpected rewards (prediction error)
    // and phasically respond to reward cues
    
    // Scale reward to dopamine level (rewards can exceed 1.0)
    float rewardSignal = std::min(reward * 0.5f, 1.0f);
    
    // Update dopamine level with reward burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    
    if (error > 0.0f) {
        // Positive prediction error: burst of dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 0.5f);
    } else {
        // Negative prediction error: dip in dopamine
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate);
    }
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 0.0f, 2.0f);
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.0f, rate);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

struct Acetylcholine::Impl {
    float level;
    float attentionBoost;
    float memoryGain;
    float baseline;
    
    Impl() : level(0.0f), attentionBoost(1.5f), memoryGain(1.2f), baseline(0.0f) {}
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

NeuromodulatorType Acetylcholine::getType() const {
    return NeuromodulatorType::Acetylcholine;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine facilitates synaptic plasticity and memory encoding
    return 0.5f + pImpl->level * 0.5f;  // Increases with level
}

void Acetylcholine::update(TimestepDuration dt) {
    // Simple decay for now
    float decay = 0.05f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::applyToNeuron(Neuron* neuron, float factor) {
    if (!neuron) return;
    
    // Acetylcholine modulates attention and sensory processing
    // Enhances signal-to-noise ratio in cortical circuits
    neuron->injectCurrent(factor * pImpl->attentionBoost);
}

void Acetylcholine::applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) {
    // ACh enhances both LTP and memory consolidation
    ltpWeight *= pImpl->memoryGain;
    // LTD less affected by ACh
    ltdWeight *= 1.1f;
}

void Acetylcholine::enhanceAttention() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
}

void Acetylcholine::facilitateMemoryEncoding() {
    // Placeholder for memory encoding enhancement
    // Would modulate synaptic tagging and capture processes
}

void Acetylcholine::modulateSensoryProcessing(float gain) {
    // Modulate gain of sensory inputs
    pImpl->attentionBoost = 1.0f + gain;
}

struct Norepinephrine::Impl {
    float level;
    float focusEnhancement;
    float arousalLevel;
    float baseline;
    float workingMemoryCapacity;
    
    Impl() : level(0.0f), focusEnhancement(1.3f), arousalLevel(0.5f), baseline(0.0f), workingMemoryCapacity(1.0f) {}
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

NeuromodulatorType Norepinephrine::getType() const {
    return NeuromodulatorType::Norepinephrine;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE has complex effects on plasticity - depends on concentration and receptor type
    if (pImpl->level < 0.3f) {
        // Low NE: reduces plasticity (fight or flight)
        return 0.3f;
    } else if (pImpl->level < 0.7f) {
        // Medium NE: normalizes plasticity
        return 0.6f + pImpl->level * 0.2f;
    } else {
        // High NE: can enhance or impair plasticity depending on context
        return 0.8f;
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics involve rapid release and slower clearance
    float decay = 0.02f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::applyToNeuron(Neuron* neuron, float factor) {
    if (!neuron) return;
    
    // NE modulates arousal, focus, and working memory
    // Acts through alpha and beta receptors with different effects
    neuron->injectCurrent(factor * pImpl->focusEnhancement);
    
    // Enhance working memory capacity
    if (pImpl->level > 0.5f) {
        // Would modulate working memory parameters here
    }
}

void Norepinephrine::applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) {
    // NE optimizes plasticity for learning in novel or salient situations
    if (pImpl->level > 0.3f) {
        // Enhance LTP for salient stimuli
        ltpWeight *= 1.2f;
        // Suppress LTD for stable memory formation
        ltdWeight *= 0.9f;
    }
}

void Norepinephrine::increaseArousal() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.15f);
}

void Norepinephrine::enhanceFocus() {
    pImpl->focusEnhancement = 1.5f;
}

void Norepinephrine::modulateLiking() {
    // Placeholder for reward/liking modulation
}

void Norepinephrine::improveWorkingMemory(float capacityIncrease) {
    pImpl->workingMemoryCapacity = std::min(2.0f, pImpl->workingMemoryCapacity + capacityIncrease);
}

struct Serotonin::Impl {
    float level;
    float moodEnhancement;
    float impulsivityReduction;
    float socialityBoost;
    float baseline;
    
    Impl() : level(0.0f), moodEnhancement(1.2f), impulsivityReduction(0.8f), 
             socialityBoost(1.1f), baseline(0.0f) {}
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

NeuromodulatorType Serotonin::getType() const {
    return NeuromodulatorType::Serotonin;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin generally modulates plasticity in complex ways
    // Low serotonin: increased impulsivity and risk-taking
    // High serotonin: enhanced patience and long-term planning
    if (pImpl->level < 0.3f) {
        return 1.3f;  // Increased plasticity (impulsive learning)
    } else {
        return 0.7f;  // Decreased plasticity (reflective learning)
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin has longer time constants than other neuromodulators
    float decay = 0.01f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::applyToNeuron(Neuron* neuron, float factor) {
    if (!neuron) return;
    
    // Serotonin modulates mood, impulsivity, and social behavior
    // Acts through multiple receptor subtypes with diverse effects
    neuron->injectCurrent(factor * pImpl->moodEnhancement);
}

void Serotonin::applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) {
    // Serotonin promotes behavioral inhibition and mood stability
    if (pImpl->level > 0.5f) {
        // Enhance LTD for behavioral inhibition
        ltdWeight *= 1.2f;
        // Suppress LTP for impulsive responses
        ltpWeight *= 0.9f;
    }
}

void Serotonin::improveMood() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
}

void Serotonin::decreaseImpulsivity() {
    pImpl->impulsivityReduction = 0.7f;
}

void Serotonin::modulateSocialBehavior(float sociality) {
    pImpl->socialityBoost = 1.0f + sociality;
}

void Serotonin::facilitateSensoryGating() {
    // Placeholder for sensory gating modulation
}

} // namespace nlm
