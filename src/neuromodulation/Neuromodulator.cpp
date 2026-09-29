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

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // ACh-specific parameters
    float attentionGain;        // Modulates attention to sensory input
    float memoryConsolidation;  // Affects memory consolidation
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.5f),
             attentionGain(1.0f), memoryConsolidation(0.8f) {}
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
    // TODO PHASE 2: Implement real ACh-modulated plasticity factor
    // PLACEHOLDER: Higher ACh increases learning rate for attention and memory
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real ACh dynamics
    // PLACEHOLDER: Decay towards baseline with attention modulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float salience) {
    // TODO PHASE 2: Implement real attention signaling
    // PLACEHOLDER: ACh released when attention is focused
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->releaseRate * 1.5f);
}

void Acetylcholine::signalNovelty(float novelty) {
    // TODO PHASE 2: Implement novelty detection signaling
    // PLACEHOLDER: ACh responds to novel stimuli for exploration
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.8f);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // NE-specific parameters
    float arousalLevel;        // Modulates overall arousal and vigilance
    float locusCoeruleusGain;   // Modulates LC neuron firing
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.08f), releaseRate(1.2f),
             arousalLevel(1.0f), locusCoeruleusGain(1.0f) {}
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
    // TODO PHASE 2: Implement real NE-modulated plasticity factor
    // PLACEHOLDER: Higher NE increases plasticity for novel learning
    return 0.6f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real NE dynamics
    // PLACEHOLDER: Decay towards baseline with arousal modulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // TODO PHASE 2: Implement arousal signaling
    // PLACEHOLDER: NE released during high arousal states
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate * 2.0f);
}

void Norepinephrine::signalStress(float stress) {
    // TODO PHASE 2: Implement stress signaling
    // PLACEHOLDER: NE responds to stress and alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate * 1.5f);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // 5-HT-specific parameters
    float mood;                 // Modulates mood and emotional state
    float impulsivity;          // Controls exploratory behavior
    float socialCohesion;       // Affects social behavior
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.12f), releaseRate(1.0f),
             mood(0.5f), impulsivity(0.5f), socialCohesion(0.5f) {}
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
    // TODO PHASE 2: Implement real 5-HT-modulated plasticity factor
    // PLACEHOLDER: Higher 5-HT modulates plasticity for emotional learning
    return 0.7f + 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real 5-HT dynamics
    // PLACEHOLDER: Decay towards baseline with mood modulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalReward(float reward) {
    // TODO PHASE 2: Implement reward signaling
    // PLACEHOLDER: 5-HT released on reward and increases with consistency
    float rewardBoost = reward > 0.5f ? 1.5f : 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * rewardBoost);
}

void Serotonin::signalSocial(float social) {
    // TODO PHASE 2: Implement social signaling
    // PLACEHOLDER: 5-HT increases with positive social interactions
    pImpl->level = std::min(pImpl->peak, pImpl->level + social * pImpl->releaseRate * 1.3f);
}

} // namespace nlm
