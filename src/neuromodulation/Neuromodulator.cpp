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

void Dopamine::signalPredictionError(float error) {
    // Signal prediction error for learning
    // TODO PHASE 2: Implement real prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    if (error > 0) {  // Positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate);
    } else if (error < 0) {  // Negative prediction error (worse than expected)
        pImpl->level = std::max(pImpl->baseline, pImpl->level - std::abs(error) * pImpl->decayRate * 0.5f);
    }
}

void Acetylcholine::signalPredictionError(float error) {
    // Acetylcholine modulates attention and memory based on prediction error
    // PLACEHOLDER: Implement attention modulation
    if (error < 0) {  // Negative prediction error (unexpected)
        // Increase attention to unexpected events
        setLevel(std::min(1.0f, getLevel() + 0.1f * std::abs(error)));
    }
}

void Norepinephrine::signalPredictionError(float error) {
    // Norepinephrine increases arousal based on prediction error
    // PLACEHOLDER: Implement arousal modulation
    if (error < 0) {  // Negative prediction error
        // Increase alertness for unexpected events
        setLevel(std::min(1.0f, getLevel() + 0.05f * std::abs(error)));
    }
}

void Serotonin::signalPredictionError(float error) {
    // Serotonin affects mood and impulsivity based on prediction error
    // PLACEHOLDER: Implement mood modulation
    if (error < 0) {  // Negative prediction error (bad news)
        // Decrease serotonin for unexpected negative events
        setLevel(std::max(0.0f, getLevel() - 0.05f * std::abs(error)));
    }
}

// Acetylcholine class implementation

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
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
    // Acetylcholine modulates attention and memory consolidation
    // Higher ACh improves working memory and attention
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay acetylcholine level over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Norepinephrine class implementation

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
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
    // Norepinephrine affects arousal and vigilance
    // Higher NE improves signal detection
    return 1.0f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay norepinephrine level over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Serotonin class implementation

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f) {}
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
    // Serotonin affects mood and impulsivity
    // Higher serotonin reduces impulsivity and improves stability
    return 1.0f - 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay serotonin level over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

