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
    // Enhanced dopamine plasticity factor with saturation
    float basePlasticity = 0.5f;
    float dynamicFactor = 1.0f + pImpl->level * 0.8f;  // Higher dopamine = higher plasticity
    
    // Apply metaplastic modulation based on prediction error (would need external access)
    return basePlasticity * dynamicFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with adaptive baseline and prediction error integration
    float decay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline with homeostatic regulation
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    } else if (pImpl->level < pImpl->baseline) {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + decay * 0.5f);
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement real reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

struct Acetylcholine::Impl {
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float level;
    
    Impl() : baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f), level(0.0f) {}
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
    // ACh enhances plasticity for learning and memory consolidation
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays slower than dopamine, supports sustained attention
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::enhanceAttention(float gain) {
    setLevel(std::min(1.0f, getLevel() + gain * 0.5f));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Boost ACh level to support memory consolidation
    if (getLevel() < 0.7f) {
        setLevel(getLevel() + 0.3f);
    }
}

struct Norepinephrine::Impl {
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float level;
    
    Impl() : baseline(0.0f), peak(1.0f), decayRate(0.03f), releaseRate(1.5f), level(0.0f) {}
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
    // NE enhances signal-to-noise ratio and working memory
    return 1.0f + pImpl->level * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics support arousal and vigilance
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::enhanceArousal(float level) {
    setLevel(std::min(1.0f, getLevel() + level * 0.5f));
}

void Norepinephrine::enhanceVigilance() {
    // Boost NE level to increase alertness
    if (getLevel() < 0.5f) {
        setLevel(getLevel() + 0.3f);
    }
}

struct Serotonin::Impl {
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float level;
    
    Impl() : baseline(0.5f), peak(1.0f), decayRate(0.08f), releaseRate(0.8f), level(0.5f) {}
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
    // Serotonin modulates plasticity to balance exploration and exploitation
    // Higher levels reduce plasticity for stability
    return 1.0f - pImpl->level * 0.4f;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin maintains mood homeostasis
    float decay = pImpl->decayRate * static_cast<float>(dt);
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    } else {
        pImpl->level = std::min(pImpl->baseline, pImpl->level + decay * 0.5f);
    }
}

void Serotonin::modulateMood(float level) {
    setLevel(std::min(1.0f, getLevel() + level * 0.3f));
}

void Serotonin::reduceImpulsivity(float level) {
    if (getLevel() > 0.2f) {
        setLevel(getLevel() - level * 0.2f);
    }
}

} // namespace nlm
