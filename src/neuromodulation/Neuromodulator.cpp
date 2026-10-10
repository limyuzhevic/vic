// Neuromodulator.cpp
// Implementation of enhanced neuromodulation systems

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Dopamine implementation (enhanced)
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityMultiplier;
    float predictionErrorSensitivity;
    float rewardSignalStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             plasticityMultiplier(1.0f), predictionErrorSensitivity(1.0f), rewardSignalStrength(1.0f) {}
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
    // Enhanced dopamine modulation with prediction error sensitivity
    float rewardEffect = 0.3f + 0.7f * pImpl->level * pImpl->rewardSignalStrength;
    float predictionEffect = 0.5f + 0.5f * pImpl->level * pImpl->predictionErrorSensitivity;
    return rewardEffect * predictionEffect;
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with more nuanced decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with learning modulation
    float burst = reward * pImpl->releaseRate * pImpl->rewardSignalStrength;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced reward prediction error signaling with faster dynamics
    float errorBoost = error * pImpl->releaseRate * 0.5f * pImpl->predictionErrorSensitivity;
    if (error > 0.0f) {
        // Positive prediction error (better than expected): burst of dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorBoost * 2.0f);
    } else {
        // Negative prediction error (worse than expected): suppression
        pImpl->level = std::max(pImpl->baseline, pImpl->level + errorBoost);
    }
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f), attentionGain(1.0f) {}
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
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost() {
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
}

void Acetylcholine::signalLearning(float learningStrength) {
    setLevel(learningStrength * 0.8f);
}

void Acetylcholine::updateAttentionParameters(float gain, float threshold) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 2.0f);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalFactor;
    float vigilanceFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f), releaseRate(1.0f), 
             arousalFactor(1.0f), vigilanceFactor(1.0f) {}
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
    float arousalEffect = 0.3f + 0.7f * pImpl->level * pImpl->arousalFactor;
    float vigilanceEffect = 0.5f + 0.5f * pImpl->level * pImpl->vigilanceFactor;
    return arousalEffect * vigilanceEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel, float stressLevel) {
    float boost = arousalLevel * 0.5f + stressLevel * 0.3f;
    setLevel(getLevel() + boost * pImpl->releaseRate);
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    if (vigilanceLevel > 0.5f) {
        float burst = (vigilanceLevel - 0.5f) * 1.0f;
        setLevel(std::min(pImpl->peak, getLevel() + burst * pImpl->releaseRate * 2.0f));
    }
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodFactor;
    float impulsivityFactor;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.02f), releaseRate(1.0f),
             moodFactor(1.0f), impulsivityFactor(1.0f) {}
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
    float moodEffect = 0.5f + 0.5f * pImpl->level * pImpl->moodFactor;
    float impulsivityEffect = 0.8f - 0.3f * pImpl->level * pImpl->impulsivityFactor;
    return moodEffect * impulsivityEffect;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel, float socialContext) {
    float boost = moodLevel * 0.6f + socialContext * 0.4f;
    setLevel(std::min(pImpl->peak, getLevel() + boost * pImpl->releaseRate));
}

void Serotonin::signalSocialStatus(float statusLevel) {
    float statusEffect = statusLevel * 0.5f;
    setLevel(getLevel() + statusEffect * pImpl->releaseRate);
}

} // namespace nlm
