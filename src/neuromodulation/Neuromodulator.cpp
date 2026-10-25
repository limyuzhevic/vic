#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<float> uniform(0.0f, 1.0f);

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float eligibilityTrace;
    float predictionError;
    Timestamp lastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             eligibilityTrace(0.0f), predictionError(0.0f), lastUpdate(0.0) {}
};

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float memoryBoost;
    Timestamp lastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f),
             attentionBoost(1.2f), memoryBoost(1.5f), lastUpdate(0.0) {}
};

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float tonicLevel;
    Timestamp lastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.2f), releaseRate(0.6f),
             arousal(1.5f), vigilance(1.3f), tonicLevel(0.1f), lastUpdate(0.0) {}
};

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float impulsivity;
    float mood;
    float socialLearning;
    Timestamp lastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(0.7f),
             impulsivity(1.1f), mood(1.0f), socialLearning(1.2f), lastUpdate(0.0) {}
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
    // Enhanced dopamine effects on plasticity
    // More sophisticated plasticity modulation based on dopamine level
    float basePlasticity = 0.5f;
    float dynamicModulation = pImpl->level * 0.5f;
    float eligibilityTraceMod = pImpl->eligibilityTrace * 0.3f;
    float predictionErrorMod = pImpl->predictionError * 0.2f;
    
    float plasticityFactor = basePlasticity + dynamicModulation + eligibilityTraceMod - predictionErrorMod;
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics with baseline decay and activity-dependent recovery
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->eligibilityTrace *= std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->predictionError *= std::exp(-pImpl->decayRate * 2.0f * static_cast<float>(dt));
    pImpl->lastUpdate = static_cast<Timestamp>(dt);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling with peak limiting and temporal dynamics
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->predictionError = 0.0f; // Reward resets prediction error
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + reward * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling - positive for better-than-expected, negative for worse
    pImpl->predictionError = error;
    if (error > 0) {
        // Positive prediction error: increased dopamine level
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 2.0f);
    } else if (error < 0) {
        // Negative prediction error: decreased dopamine level but not below zero
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    }
}

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
    // Higher ACh improves attention focus and memory stabilization
    float basePlasticity = 1.0f;
    float attentionMod = pImpl->level * (pImpl->attentionBoost - 1.0f);
    float memoryMod = pImpl->level * (pImpl->memoryBoost - 1.0f);
    
    return basePlasticity + attentionMod + memoryMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine dynamics with learning-related fluctuations
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->lastUpdate = static_cast<Timestamp>(dt);
}

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
    // Norepinephrine modulates arousal and vigilance
    // Affects attention breadth and learning under stress
    float basePlasticity = 1.0f;
    float arousalMod = pImpl->level * (pImpl->arousal - 1.0f);
    float vigilanceMod = pImpl->level * (pImpl->vigilance - 1.0f);
    float tonicMod = pImpl->tonicLevel;
    
    return basePlasticity + arousalMod + vigilanceMod - tonicMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine dynamics with tonic baseline and phasic responses
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->lastUpdate = static_cast<Timestamp>(dt);
}

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
    // Serotonin modulates mood, impulsivity, and social learning
    // Affects risk-taking and exploratory behavior
    float basePlasticity = 1.0f;
    float moodMod = pImpl->level * (pImpl->mood - 1.0f);
    float impulsivityMod = pImpl->level * (pImpl->impulsivity - 1.0f);
    float socialMod = pImpl->level * (pImpl->socialLearning - 1.0f);
    
    return basePlasticity + moodMod + impulsivityMod + socialMod;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics with homeostatic regulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->lastUpdate = static_cast<Timestamp>(dt);
}

} // namespace nlm
