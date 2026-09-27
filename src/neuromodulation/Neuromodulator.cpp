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

// Serotonin: Mood, impulsivity, and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    float adaptationRate;
    float burstRate;
    float historyMax;
    
    Impl() : level(0.0f), baseline(0.5f), decayRate(0.05f), 
             adaptationRate(0.02f), burstRate(0.1f), historyMax(0.0f) {}
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
    pImpl->historyMax = std::max(pImpl->historyMax, pImpl->level);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: higher levels reduce LTP, enhance LTD
    // Inverted-U relationship: moderate levels optimal for learning
    float level = getLevel();
    if (level <= 0.3f) {
        return 0.5f + 0.8f * level;  // Increasing phase
    } else {
        return 1.0f - 0.5f * (level - 0.3f);  // Decreasing phase
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics based on behavioral context
    // Decays towards baseline, increases with positive experiences
    
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialExperience(bool positive) {
    // Social experience modulates serotonin
    if (positive) {
        // Positive social interactions increase serotonin
        pImpl->level = std::min(1.0f, pImpl->level + pImpl->burstRate);
    } else {
        // Negative experiences decrease serotonin
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->burstRate * 2.0f);
    }
}

float Serotonin::getHistoryMax() const {
    return pImpl->historyMax;
}

void Serotonin::resetHistoryMax() {
    pImpl->historyMax = 0.0f;
}

// Acetylcholine: Attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    float burstRate;
    float attentionWeight;
    float memoryGain;
    
    Impl() : level(0.0f), baseline(0.1f), decayRate(0.05f),
             burstRate(0.15f), attentionWeight(1.0f), memoryGain(0.8f) {}
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
    // Acetylcholine modulates attention and working memory
    // Higher levels enhance encoding and retrieval
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: increases with attention demand, decays naturally
    // Natural decay
    pImpl->level = std::max(pImpl->baseline,
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionScore) {
    // Attention modulates acetylcholine release
    pImpl->level = std::min(1.0f, pImpl->level + attentionScore * pImpl->burstRate);
}

void Acetylcholine::signalMemoryEncoding() {
    // Memory encoding increases acetylcholine
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
}

float Acetylcholine::getAttentionWeight() const {
    return pImpl->attentionWeight;
}

void Acetylcholine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = weight;
}

// Noradrenaline: Arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float burstRate;
    float noveltyResponse;
    float signalDetection;
    
    Impl() : level(0.0f), baseline(0.1f), decayRate(0.03f),
             burstRate(0.2f), noveltyResponse(1.0f), signalDetection(1.0f) {}
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
    // Noradrenaline modulates arousal and signal detection
    // Inverted-U: optimal at moderate levels for learning
    float level = getLevel();
    if (level <= 0.5f) {
        return 0.5f + 0.8f * level;  // Increasing phase (arousal improves learning)
    } else {
        return 1.0f - 0.5f * (level - 0.5f);  // Decreasing phase (over-arousal impairs learning)
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics based on novel or salient stimuli
    // Natural decay, increases with novelty or surprising events
    pImpl->level = std::max(pImpl->baseline,
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalNovelty(float noveltyScore) {
    // Novel stimuli trigger noradrenaline release
    pImpl->level = std::min(1.0f, pImpl->level + noveltyScore * pImpl->burstRate);
}

void Norepinephrine::signalThreat(float threatLevel) {
    // Threat increases noradrenaline (fight or flight response)
    pImpl->level = std::min(1.0f, pImpl->level + threatLevel * pImpl->burstRate * 2.0f);
}

void Norepinephrine::signalRewardPredictionError(float error) {
    // Reward prediction error increases noradrenaline
    pImpl->level = std::min(1.0f, pImpl->level + std::abs(error) * pImpl->noveltyResponse * 0.5f);
}

float Norepinephrine::getSignalDetection() const {
    return pImpl->signalDetection;
}

void Norepinephrine::setSignalDetection(float detection) {
    pImpl->signalDetection = detection;
}
