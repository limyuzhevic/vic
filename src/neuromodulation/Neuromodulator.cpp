#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationTime;
    float eligibilityTrace;  // For reward prediction error
    float predictionError;  // For reward prediction error signaling
    float timeSinceLastEvent;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             adaptationTime(50.0f), eligibilityTrace(0.0f), predictionError(0.0f), timeSinceLastEvent(0.0f) {}
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
    // Real implementation: Dopamine modulates synaptic plasticity through receptor subtypes
    // - D1 receptors enhance LTP
    // - D2 receptors enhance LTD
    // - Net effect depends on baseline dopamine levels
    
    // More sophisticated model with receptor dynamics
    float d1Effect = 0.3f * pImpl->level;    // D1 receptor effect (LTP enhancement)
    float d2Effect = 0.3f * (1.0f - pImpl->level);  // D2 receptor effect (LTD enhancement)
    
    // Net plasticity factor balances excitation and inhibition
    float netFactor = 0.5f + (d1Effect - d2Effect) * 0.4f;
    
    // Ensure factor stays within reasonable bounds
    return std::clamp(netFactor, 0.1f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    // Real implementation: Realistic dopamine dynamics based on neuroscience
    
    pImpl->timeSinceLastEvent += static_cast<float>(dt);
    
    // Natural decay with time constant
    float decayFactor = std::exp(-static_cast<float>(dt) / pImpl->decayRate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Eligibility trace decay
    pImpl->eligibilityTrace *= decayFactor;
    
    // Prediction error decay
    pImpl->predictionError *= decayFactor;
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    // Real implementation: Reward prediction error signaling based on dopamine bursts
    
    // Compute reward prediction error (simplified)
    float predictionError = reward - pImpl->predictionError;
    
    // Dopamine burst proportional to prediction error
    float burstMagnitude = std::min(1.0f, std::abs(predictionError) * 2.0f);
    
    if (predictionError > 0) {
        // Positive prediction error: reward prediction
        pImpl->level += burstMagnitude * pImpl->releaseRate;
    } else {
        // Negative prediction error: omission prediction
        pImpl->level = std::max(0.0f, pImpl->level - burstMagnitude * pImpl->releaseRate * 0.5f);
    }
    
    // Update eligibility trace for future plasticity
    pImpl->eligibilityTrace += burstMagnitude * pImpl->releaseRate * 0.1f;
    
    // Store current reward for next prediction error
    pImpl->predictionError = reward;
    
    // Clamp level
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    // Real implementation: Dopamine signals prediction errors for learning
    
    // Update prediction error with temporal dynamics
    pImpl->predictionError = error;
    
    // Dopamine response to prediction error (TD learning rule)
    float delta = error * pImpl->eligibilityTrace;
    
    // Phasic dopamine response
    if (std::abs(delta) > 0.001f) {
        pImpl->level += delta * pImpl->releaseRate;
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
}

// Acetylcholine implementation with more realistic dynamics
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float burstMagnitude;
    float attentionModulator;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.2f), burstMagnitude(1.0f), attentionModulator(0.5f) {}
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
    // Acetylcholine modulates plasticity for memory encoding
    // - Low ACh: Enhances LTP for novel information
    // - High ACh: Enhances working memory and attention
    
    // Inverted relationship: lower ACh = higher plasticity for novel stimuli
    return 0.5f + 0.5f * (1.0f - pImpl->level);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay
    float decayFactor = std::exp(-static_cast<float>(dt) / pImpl->decayRate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
}

// Norepinephrine implementation with arousal and vigilance modulation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float locusCoeruleusGain;
    float arousalSignal;
    float vigilanceSignal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.3f), locusCoeruleusGain(1.0f),
             arousalSignal(0.5f), vigilanceSignal(0.5f) {}
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
    // Norepinephrine modulates arousal and attention
    // - Low NE: Reduced arousal, better encoding of novel stimuli
    // - High NE: High arousal, focused attention, reduced plasticity
    
    // Inverted-U relationship: optimal NE for learning
    float arousal = pImpl->level;
    float optimalArousal = 0.5f;
    float sensitivity = 2.0f;
    
    // Inverted-U function
    float plasticity = std::exp(-sensitivity * std::pow(arousal - optimalArousal, 2)) * 0.8f + 0.1f;
    
    return plasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay with burst dynamics
    float decayFactor = std::exp(-static_cast<float>(dt) / pImpl->decayRate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
}

// Serotonin implementation with mood and impulsivity modulation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float tonicLevel;
    float phasicResponse;
    float satietySignal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.4f), tonicLevel(0.3f), 
             phasicResponse(1.0f), satietySignal(0.5f) {}
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
    // Serotonin modulates mood, impulsivity, and behavioral inhibition
    // - Low serotonin: increased impulsivity, enhanced reward learning
    // - High serotonin: increased inhibition, enhanced punishment learning
    
    // Inverted relationship: lower serotonin = higher impulsivity-driven learning
    return 0.3f + 0.7f * (1.0f - pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay with homeostatic regulation
    float decayFactor = std::exp(-static_cast<float>(dt) / pImpl->decayRate);
    
    // Homeostatic regulation towards tonic level
    float homeostaticPull = pImpl->tonicLevel * 0.1f * static_cast<float>(dt);
    
    pImpl->level = pImpl->level * decayFactor + homeostaticPull;
}

} // namespace nlm
