#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ===== DOPAMINE IMPLEMENTATION =====

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    
    Impl() : level(0.0f), baseline(0.0f), peak(2.0f), decayRate(0.1f), releaseRate(1.0f), predictionError(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: Higher dopamine increases plasticity up to a point
    // But very high dopamine can actually decrease stability
    if (pImpl->level < 0.5f) {
        return 0.5f + pImpl->level * 1.0f;  // Linear increase at low levels
    } else {
        return 1.5f - (pImpl->level - 0.5f) * 0.5f;  // Diminishing returns at high levels
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: decay towards baseline with some stochasticity
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add small noise for biological realism
    float noise = ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 0.05f * pImpl->level;
    pImpl->level += noise;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic response to unexpected rewards
    // Standard reward prediction error: δ = reward - V(s)
    float predictionError = reward - pImpl->predictionError;
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate);
    pImpl->predictionError = reward;  // Update expected reward
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: direct response to error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->predictionError = error;  // Store the prediction error
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

// ===== ACETYLCHOLINE IMPLEMENTATION =====

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.5f), decayRate(0.08f), attentionGain(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()), attentionMod_(0.0f), memoryConsolidationBoost_(0.0f) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.5f);
    // Update modulation effects based on level
    attentionMod_ = std::min(1.0f, pImpl->level * 0.8f);  // 80% of ACh level for attention
    memoryConsolidationBoost_ = std::min(0.5f, pImpl->level * 0.3f);  // 30% of ACh level for memory
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity indirectly through attention and memory
    return 1.0f + attentionMod_ * 0.2f;  // 0-20% increase in effective plasticity
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh dynamics: slower decay, promotes memory encoding
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // ACh enhances memory consolidation during rest periods
    // This effect is applied externally by the brain
}

// ===== NOREPINEPHRINE IMPLEMENTATION =====

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f), arousalRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()), arousal_(0.0f), planningMod_(0.0f) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update arousal and planning modulation based on level
    arousal_ = pImpl->level;  // Arousal directly related to NE level
    planningMod_ = pImpl->level * 0.5f;  // Planning modulation is 50% of NE level
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and focus, affecting effective learning rate
    return 1.0f + arousal_ * 0.3f;  // 0-30% increase in effective learning rate
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE dynamics: faster decay, promotes vigilance and focus
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // NE enhances signal-to-noise ratio in neural circuits
    // This effect is applied externally by the brain
}

// ===== SEROTONIN IMPLEMENTATION =====

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), moodRate(1.0f) {}
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()), mood_(0.0f), explorationBias_(0.0f) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update mood and exploration bias based on level
    mood_ = pImpl->level;  // Mood directly related to 5-HT level
    explorationBias_ = (1.0f - pImpl->level) * 0.4f;  // Higher 5-HT = less exploration
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates mood and behavioral flexibility
    return 1.0f + mood_ * 0.2f;  // 0-20% increase based on mood
}

void Serotonin::update(TimestepDuration dt) {
    // Real 5-HT dynamics: slowest decay, regulates behavioral state
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // 5-HT regulates exploration-exploitation balance
    // This effect is applied externally by the brain
}

} // namespace nlm
