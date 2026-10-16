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
    float encodingGain;
    float attentionGain;
    float decayRate;
    
    Impl() : level(0.0f), encodingGain(1.0f), attentionGain(1.0f), decayRate(0.1f) {}
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
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceEncoding(float strength) {
    pImpl->encodingGain = std::min(2.0f, 1.0f + strength * 0.5f);
}

void Acetylcholine::modulateAttention(float gain) {
    pImpl->attentionGain = std::min(3.0f, 1.0f + gain * 0.5f);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float competitionThreshold;
    float decayRate;
    
    Impl() : level(0.0f), arousalLevel(0.5f), competitionThreshold(0.5f), decayRate(0.1f) {}
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
    return 0.6f + 0.4f * pImpl->arousalLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::regulateArousal(float arousalLevel) {
    pImpl->arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
}

void Norepinephrine::setCompetitionThreshold(float threshold) {
    pImpl->competitionThreshold = std::clamp(threshold, 0.1f, 1.0f);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float rewardPredictionError;
    float consolidationStrength;
    float decayRate;
    
    Impl() : level(0.0f), rewardPredictionError(0.0f), consolidationStrength(1.0f), decayRate(0.1f) {}
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
    return 0.4f + 0.6f * pImpl->consolidationStrength;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateRewardPredictionError(float error) {
    pImpl->rewardPredictionError = std::clamp(error, -1.0f, 1.0f);
}

void Serotonin::consolidateMemory(float consolidationStrength) {
    pImpl->consolidationStrength = std::clamp(consolidationStrength, 0.1f, 2.0f);
}

// Novelty implementation
struct Novelty::Impl {
    float level;
    float encodingBoost;
    float capacityBoost;
    float decayRate;
    
    Impl() : level(0.0f), encodingBoost(1.0f), capacityBoost(1.0f), decayRate(0.1f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "NOVELTY";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    return 0.7f + 0.3f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::enhanceEncodingCapacity(float factor) {
    pImpl->capacityBoost = std::min(3.0f, 1.0f + factor * 0.5f);
}

void Novelty::boostPatternEncoding(float strength) {
    pImpl->encodingBoost = std::min(2.0f, 1.0f + strength * 0.5f);
}

} // namespace nlm
