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
    float plasticityMod;
    float memoryMod;
    float attentionMod;
    
    // Integration state
    float achInfluence;
    float neInfluence;
    float serotoninInfluence;
    
    // Prediction tracking
    float lastPredictionError;
    std::vector<float> predictionErrorHistory;
    size_t historyIndex;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             plasticityMod(1.0f), memoryMod(0.5f), attentionMod(0.5f),
             achInfluence(0.0f), neInfluence(0.0f), serotoninInfluence(0.0f),
             lastPredictionError(0.0f), historyIndex(0) {
        predictionErrorHistory.resize(100, 0.0f);
    }
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
    return pImpl->plasticityMod;
}

void Dopamine::update(float dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt);
    
    // Update integration influences
    updateIntegration(dt);
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Update prediction error
    float predictionError = reward - pImpl->lastPredictionError;
    pImpl->predictionErrorHistory[pImpl->historyIndex] = predictionError;
    pImpl->lastPredictionError = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Store prediction error
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
}

void Dopamine::integrateAcetylcholine(float achLevel) {
    // ACh modulates dopamine release
    pImpl->achInfluence = achLevel * 0.3f;
}

void Dopamine::integrateNorepinephrine(float neLevel) {
    // NE modulates dopamine baseline
    pImpl->neInfluence = neLevel * 0.2f;
}

void Dopamine::integrateSerotonin(float serotoninLevel) {
    // Serotonin modulates dopamine plasticity
    pImpl->serotoninInfluence = serotoninLevel * 0.2f;
}

float Dopamine::getMemoryModulationFactor() const {
    return pImpl->memoryMod;
}

float Dopamine::getAttentionModulationFactor() const {
    return pImpl->attentionMod;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->achInfluence = 0.0f;
    pImpl->neInfluence = 0.0f;
    pImpl->serotoninInfluence = 0.0f;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory consolidation
    bool consolidationEnabled;
    float consolidationStrength;
    
    // Attention
    float attentionBias;
    float globalAttention;
    
    // Integration weights
    float daWeight;
    float neWeight;
    float serotoninWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             consolidationEnabled(true), consolidationStrength(0.8f), attentionBias(0.5f),
             globalAttention(0.5f), daWeight(0.3f), neWeight(0.2f), serotoninWeight(0.1f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::setLevel(float level) {
    pImpl->level = level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention and learning
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(float dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt);
}

void Acetylcholine::integrateDopamine(float daLevel) {
    // DA modulates ACh release
    pImpl->globalAttention += daLevel * pImpl->daWeight * 0.1f;
}

void Acetylcholine::integrateNorepinephrine(float neLevel) {
    // NE modulates ACh baseline
    pImpl->attentionBias += neLevel * pImpl->neWeight * 0.05f;
}

void Acetylcholine::integrateSerotonin(float serotoninLevel) {
    // Serotonin modulates ACh plasticity
    pImpl->consolidationStrength += serotoninLevel * pImpl->serotoninWeight * 0.1f;
}

void Acetylcholine::enableMemoryConsolidation(bool enable) {
    pImpl->consolidationEnabled = enable;
}

void Acetylcholine::setConsolidationStrength(float strength) {
    pImpl->consolidationStrength = std::clamp(strength, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionBias() const {
    return pImpl->attentionBias;
}

void Acetylcholine::setAttentionBias(float bias) {
    pImpl->attentionBias = std::clamp(bias, 0.0f, 1.0f);
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->globalAttention = 0.5f;
    pImpl->attentionBias = 0.5f;
    pImpl->consolidationStrength = 0.8f;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal
    float arousalLevel;
    float targetArousal;
    
    // Vigilance
    float vigilance;
    float lastStimulusTime;
    
    // Stress
    bool underStress;
    float stressIntensity;
    float stressDuration;
    
    // Integration weights
    float daWeight;
    float achWeight;
    float serotoninWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(0.5f), targetArousal(0.5f), vigilance(0.5f), lastStimulusTime(0.0f),
             underStress(false), stressIntensity(0.0f), stressDuration(0.0f),
             daWeight(0.2f), achWeight(0.2f), serotoninWeight(0.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::setLevel(float level) {
    pImpl->level = level;
}

float Norepinephrine::getPlasticityFactor() const {
    return 1.0f + pImpl->level * 0.3f;
}

void Norepinephrine::update(float dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt);
    
    // Update arousal
    pImpl->arousalLevel += (pImpl->targetArousal - pImpl->arousalLevel) * 0.1f;
    
    // Update vigilance
    float timeSinceStimulus = dt - (static_cast<float>(std::time(nullptr)) - pImpl->lastStimulusTime);
    if (timeSinceStimulus < 0.0f) {
        pImpl->vigilance = std::max(0.1f, pImpl->vigilance - 0.01f);
    }
}

void Norepinephrine::integrateDopamine(float daLevel) {
    // DA modulates NE release
    pImpl->targetArousal += daLevel * pImpl->daWeight * 0.1f;
}

void Norepinephrine::integrateAcetylcholine(float achLevel) {
    // ACh modulates NE baseline
    pImpl->arousalLevel += achLevel * pImpl->achWeight * 0.05f;
}

void Norepinephrine::integrateSerotonin(float serotoninLevel) {
    // Serotonin modulates NE plasticity
    pImpl->stressIntensity += serotoninLevel * pImpl->serotoninWeight * 0.1f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::setArousalLevel(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

void Norepinephrine::updateVigilance(float dt) {
    pImpl->vigilance += (pImpl->arousalLevel - pImpl->vigilance) * 0.1f;
}

void Norepinephrine::triggerStress(float intensity) {
    pImpl->underStress = true;
    pImpl->stressIntensity = intensity;
    pImpl->stressDuration = 10.0f; // 10 time units
}

bool Norepinephrine::isUnderStress() const {
    return pImpl->underStress;
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->arousalLevel = 0.5f;
    pImpl->targetArousal = 0.5f;
    pImpl->vigilance = 0.5f;
    pImpl->underStress = false;
    pImpl->stressIntensity = 0.0f;
    pImpl->stressDuration = 0.0f;
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood
    float moodLevel;
    float moodStability;
    
    // Impulsivity
    float impulsivity;
    float lastRewardDelay;
    
    // Social motivation
    float socialMotivation;
    float socialLearningRate;
    
    // Integration weights
    float daWeight;
    float achWeight;
    float neWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodLevel(0.5f), moodStability(0.8f), impulsivity(0.5f), lastRewardDelay(0.0f),
             socialMotivation(0.5f), socialLearningRate(0.1f),
             daWeight(0.2f), achWeight(0.2f), neWeight(0.2f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::setLevel(float level) {
    pImpl->level = level;
}

float Serotonin::getPlasticityFactor() const {
    return 1.0f + pImpl->level * 0.2f;
}

void Serotonin::update(float dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt);
}

void Serotonin::integrateDopamine(float daLevel) {
    // DA modulates serotonin release
    pImpl->socialMotivation += daLevel * pImpl->daWeight * 0.1f;
}

void Serotonin::integrateAcetylcholine(float achLevel) {
    // ACh modulates serotonin baseline
    pImpl->moodLevel += achLevel * pImpl->achWeight * 0.05f;
}

void Serotonin::integrateNorepinephrine(float neLevel) {
    // NE modulates serotonin plasticity
    pImpl->impulsivity += neLevel * pImpl->neWeight * 0.05f;
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

void Serotonin::setMoodLevel(float mood) {
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::updateImpulsivity(float dt) {
    pImpl->impulsivity += (pImpl->moodLevel - pImpl->impulsivity) * 0.05f;
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->impulsivity -= amount * 0.1f;
}

float Serotonin::getSocialMotivation() const {
    return pImpl->socialMotivation;
}

void Serotonin::updateSocialMotivation(float dt) {
    pImpl->socialMotivation += (pImpl->level - pImpl->socialMotivation) * pImpl->socialLearningRate;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->moodStability = 0.8f;
    pImpl->impulsivity = 0.5f;
    pImpl->socialMotivation = 0.5f;
}

} // namespace nlm
