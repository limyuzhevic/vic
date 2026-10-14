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
    // Enhanced dopamine-modulated plasticity factor
    // High dopamine increases both LTP and LTD strength, but with different patterns
    return 0.3f + 0.7f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with baseline maintenance
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with burst and persistence
    float burst = reward * pImpl->releaseRate * 2.0f;  // Stronger burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Add to baseline for longer persistence
    if (reward > 0.5f) {
        pImpl->baseline += reward * 0.1f;
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced reward prediction error signaling
    // Stronger response to larger errors
    float errorResponse = error * pImpl->releaseRate * (1.0f + std::abs(error));
    pImpl->level = std::max(0.0f, pImpl->level + errorResponse);
}

// Implement Acetylcholine with attention and memory consolidation effects
struct Acetylcholine::Impl {
    float level;
    float attentionGain;
    float memoryConsolidation;
    float acetylcholineTime;
    
    Impl() : level(0.0f), attentionGain(1.0f), memoryConsolidation(0.0f), acetylcholineTime(0.0f) {}
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
    
    // Update attention and memory consolidation based on level
    pImpl->attentionGain = 0.5f + 0.5f * pImpl->level;
    pImpl->memoryConsolidation = pImpl->level * 0.8f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates synaptic consolidation and attention-based plasticity
    // Higher ACh increases both attention strength and memory consolidation
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics for attention and memory consolidation
    pImpl->level *= std::pow(0.98f, static_cast<float>(dt));
    
    // Gradual memory consolidation effect
    pImpl->memoryConsolidation = std::max(0.0f, pImpl->memoryConsolidation - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float stimulusSalience) {
    // Enhance attention based on stimulus salience
    setLevel(getLevel() + stimulusSalience * 0.3f);
}

void Acetylcholine::triggerMemoryConsolidation() {
    // Trigger memory consolidation process
    pImpl->memoryConsolidation = 1.0f;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

// Implement Norepinephrine with arousal and vigilance effects
struct Norepinephrine::Impl {
    float level;
    float arousal;
    float vigilance;
    float locusCoeruleusTime;
    
    Impl() : level(0.0f), arousal(0.0f), vigilance(0.0f), locusCoeruleusTime(0.0f) {}
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
    
    // Update arousal and vigilance based on level
    pImpl->arousal = 0.3f + 0.7f * pImpl->level;
    pImpl->vigilance = pImpl->level * 1.5f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal-based plasticity and stress-induced changes
    return 1.0f + 0.5f * pImpl->level * (1.0f - pImpl->vigilance);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics for arousal and vigilance
    pImpl->level *= std::pow(0.95f, static_cast<float>(dt));
    
    // Gradual arousal decay
    pImpl->arousal = std::max(0.3f, pImpl->arousal - 0.05f * static_cast<float>(dt));
    
    // Gradual vigilance decay
    pImpl->vigilance = std::max(0.0f, pImpl->vigilance - 0.02f * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stressLevel) {
    // Enhance arousal and vigilance under stress
    setLevel(getLevel() + stressLevel * 0.4f);
    
    // Stress enhances memory formation (fight-or-flight)
    pImpl->arousal = std::min(2.0f, pImpl->arousal + stressLevel * 0.3f);
}

void Norepinephrine::enhanceVigilance(float unexpectedStimulus) {
    // Enhance vigilance in response to unexpected stimuli
    setLevel(getLevel() + unexpectedStimulus * 0.2f);
    
    // Increased vigilance improves detection of novel stimuli
    pImpl->vigilance = std::min(2.0f, pImpl->vigilance + unexpectedStimulus * 0.1f);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

// Implement Serotonin with mood, impulsivity, and social behavior effects
struct Serotonin::Impl {
    float level;
    float mood;
    float impulseControl;
    float socialBehavior;
    float raphe NucleiTime;
    
    Impl() : level(0.0f), mood(0.5f), impulseControl(1.0f), socialBehavior(0.5f), rapheNucleiTime(0.0f) {}
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
    
    // Update mood, impulse control, and social behavior based on level
    pImpl->mood = 0.3f + 0.7f * pImpl->level;
    pImpl->impulseControl = 1.0f - 0.8f * pImpl->level;
    pImpl->socialBehavior = 0.5f + 0.5f * pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood-based plasticity and social learning
    return 0.9f + 0.1f * pImpl->level * (1.0f - std::abs(pImpl->mood - 0.5f));
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics for mood regulation
    pImpl->level *= std::pow(0.96f, static_cast<float>(dt));
    
    // Gradual mood recovery
    pImpl->mood = std::max(0.3f, pImpl->mood - 0.02f * static_cast<float>(dt));
    
    // Gradual impulse control recovery
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + 0.01f * static_cast<float>(dt));
    
    // Gradual social behavior recovery
    pImpl->socialBehavior = std::max(0.5f, pImpl->socialBehavior - 0.01f * static_cast<float>(dt));
}

void Serotonin::signalPositiveEvent(float reward) {
    // Enhance mood and social behavior with positive events
    setLevel(getLevel() + reward * 0.3f);
    
    // Positive events improve mood and impulse control
    pImpl->mood = std::min(1.0f, pImpl->mood + reward * 0.2f);
    pImpl->impulseControl = std::max(0.2f, pImpl->impulseControl + reward * 0.1f);
}

void Serotonin::signalSocialInteraction(float socialQuality) {
    // Enhance social behavior based on social interaction quality
    setLevel(getLevel() + socialQuality * 0.2f);
    
    // Social interactions improve social behavior
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialQuality * 0.15f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

} // namespace nlm
