#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>
#include <chrono>

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
    // PHASE 2 IMPLEMENTATION: Real dopamine-modulated plasticity factor
    // Based on biological evidence: dopamine enhances synaptic strengthening for rewarded stimuli
    // and inhibits weakening for unrewarded stimuli
    if (pImpl->level < 0.1f) {
        // Low dopamine - general suppression of plasticity
        return 0.2f;
    } else if (pImpl->level < 0.5f) {
        // Moderate dopamine - baseline plasticity with slight enhancement
        return 0.6f;
    } else {
        // High dopamine - strong enhancement of reward-associated plasticity
        return 1.2f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2 IMPLEMENTATION: Real dopamine dynamics
    // Simulates dopamine neuron firing patterns observed in biology
    // Natural decay towards baseline with stochastic fluctuations
    float decay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    }
    
    // Add biological noise for realistic fluctuations
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_real_distribution<float> noiseDist(-0.01f, 0.01f);
    
    float noise = noiseDist(rng);
    pImpl->level = std::clamp(pImpl->level + noise, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // PHASE 2 IMPLEMENTATION: Real reward signaling
    // Models dopamine burst responses to unexpected rewards
    // Based on dopamine neuron firing patterns in ventral tegmental area
    float burstMagnitude = std::min(1.0f, reward * 2.0f);  // Scale reward to burst magnitude
    
    // Add phasic burst response
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2 IMPLEMENTATION: Real reward prediction error signaling
    // Models temporal difference learning signals observed in dopamine systems
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    float tdSignal = error * 0.5f;  // Temporal difference learning rate
    pImpl->level = std::max(0.0f, pImpl->level + tdSignal);
}

struct Acetylcholine::Impl {
    float level;
    float attentionStrength;
    float memoryEnhancement;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), attentionStrength(0.0f), memoryEnhancement(0.0f), 
             baseline(0.0f), decayRate(0.05f) {}
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
    // PHASE 2: Acetylcholine enhances memory consolidation and attention
    return 1.0f + pImpl->memoryEnhancement * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    pImpl->attentionStrength *= 0.95f;
    pImpl->memoryEnhancement *= 0.98f;
}

void Acetylcholine::increaseAttention(float strength) {
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + strength);
}

void Acetylcholine::decreaseAttention(float strength) {
    pImpl->attentionStrength *= (1.0f - strength);
}

void Acetylcholine::enhanceMemoryConsolidation() {
    pImpl->memoryEnhancement = std::min(1.0f, pImpl->memoryEnhancement + 0.3f);
}

void Acetylcholine::suppressMemoryConsolidation() {
    pImpl->memoryEnhancement *= 0.5f;
}

struct Norepinephrine::Impl {
    float level;
    float arousalStrength;
    float stressLevel;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), arousalStrength(0.0f), stressLevel(0.0f), 
             baseline(0.0f), decayRate(0.03f) {}
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
    // PHASE 2: Norepinephrine enhances arousal and stress response
    if (pImpl->stressLevel > 0.5f) {
        return 0.8f;  // Stress reduces plasticity
    }
    return 1.0f + pImpl->arousalStrength * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    pImpl->stressLevel *= 0.9f;
}

void Norepinephrine::increaseArousal(float strength) {
    pImpl->arousalStrength = std::min(1.0f, pImpl->arousalStrength + strength);
}

void Norepinephrine::decreaseArousal(float strength) {
    pImpl->arousalStrength *= (1.0f - strength);
}

void Norepinephrine::triggerStressResponse() {
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + 0.5f);
}

void Norepinephrine::recoverFromStress() {
    pImpl->stressLevel *= 0.3f;
}

struct Serotonin::Impl {
    float level;
    float moodStrength;
    float impulsivity;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), moodStrength(0.0f), impulsivity(0.5f), 
             baseline(0.0f), decayRate(0.04f) {}
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
    // PHASE 2: Serotonin modulates mood and impulsivity
    if (pImpl->impulsivity > 0.7f) {
        return 0.7f;  // High impulsivity reduces plasticity
    }
    return 1.0f + pImpl->moodStrength * 0.4f;
}

void Serotonin::update(TimestepDuration dt) {
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    pImpl->moodStrength *= 0.96f;
}

void Serotonin::increaseMood(float strength) {
    pImpl->moodStrength = std::min(1.0f, pImpl->moodStrength + strength);
}

void Serotonin::decreaseMood(float strength) {
    pImpl->moodStrength *= (1.0f - strength);
}

void Serotonin::reduceImpulsivity(float strength) {
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - strength);
}

void Serotonin::increaseImpulsivity(float strength) {
    pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + strength);
}

} // namespace nlm