#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // ACh-specific state
    float attentionBias;          // Attention modulation
    float memoryEncodingGain;     // Memory encoding enhancement
    float learningRate;           // Learning rate modulation
    float corticalExcitability;   // Cortical excitability
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.5f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , attentionBias(1.0f)
        , memoryEncodingGain(1.0f)
        , learningRate(1.0f)
        , corticalExcitability(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

void Acetylcholine::signalAttentionGain(float gain) {
    pImpl->attentionBias = std::min(pImpl->peak, pImpl->attentionBias + gain);
}

void Acetylcholine::signalMemoryEncoding(float strength) {
    pImpl->memoryEncodingGain = std::min(pImpl->peak, pImpl->memoryEncodingGain + strength);
}

void Acetylcholine::signalLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.5f, 2.0f);
}

void Acetylcholine::signalCorticalExcitability(float excitability) {
    pImpl->corticalExcitability = std::clamp(excitability, 0.5f, 2.0f);
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->attentionBias = 1.0f + pImpl->level * 0.5f;
    pImpl->memoryEncodingGain = 1.0f + pImpl->level * 0.3f;
}

float Acetylcholine::getPlasticityFactor() const {
    return 0.7f + 0.3f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Get ACh-specific effects
float Acetylcholine::getAttentionBias() const {
    return pImpl->attentionBias;
}

float Acetylcholine::getMemoryEncodingGain() const {
    return pImpl->memoryEncodingGain;
}

float Acetylcholine::getLearningRate() const {
    return pImpl->learningRate;
}

float Acetylcholine::getCorticalExcitability() const {
    return pImpl->corticalExcitability;
}

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // NE-specific state
    float arousal;              // Arousal and vigilance
    float alertness;             // Alertness and focus
    float stressLevel;           // Stress response
    float memoryConsolidation;   // Memory consolidation
    float adaptation;            // Adaptive processing
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(2.0f)
        , decayRate(0.03f)
        , releaseRate(1.5f)
        , arousal(1.0f)
        , alertness(1.0f)
        , stressLevel(0.0f)
        , memoryConsolidation(1.0f)
        , adaptation(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

void Norepinephrine::signalArousal(float level) {
    pImpl->arousal = std::min(pImpl->peak, pImpl->arousal + level);
}

void Norepinephrine::signalAlertness(float level) {
    pImpl->alertness = std::clamp(level, 0.5f, 2.0f);
}

void Norepinephrine::signalStress(float intensity) {
    pImpl->stressLevel = std::clamp(intensity, 0.0f, 1.0f);
}

void Norepinephrine::signalMemoryConsolidation(float factor) {
    pImpl->memoryConsolidation = std::clamp(factor, 0.5f, 2.0f);
}

void Norepinephrine::signalAdaptation(float rate) {
    pImpl->adaptation = std::clamp(rate, 0.5f, 2.0f);
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->arousal = 1.0f + pImpl->level * 0.8f;
    pImpl->alertness = std::clamp(1.0f + pImpl->level * 0.5f, 0.5f, 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Get NE-specific effects
float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

float Norepinephrine::getAdaptation() const {
    return pImpl->adaptation;
}

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // 5-HT-specific state
    float mood;                  // Mood regulation
    float sleepWake;             // Sleep/wake cycle regulation
    float rewardProcessing;      // Reward processing
    float anxiety;               // Anxiety modulation
    float impulsivity;           // Impulsivity control
    float socialBehavior;        // Social behavior modulation
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.8f)
        , decayRate(0.04f)
        , releaseRate(1.2f)
        , mood(1.0f)
        , sleepWake(1.0f)
        , rewardProcessing(1.0f)
        , anxiety(0.5f)
        , impulsivity(1.0f)
        , socialBehavior(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

void Serotonin::signalMood(float level) {
    pImpl->mood = std::clamp(level, 0.3f, 2.0f);
}

void Serotonin::signalSleepWake(float cycle) {
    pImpl->sleepWake = std::clamp(cycle, 0.1f, 2.0f);
}

void Serotonin::signalRewardProcessing(float level) {
    pImpl->rewardProcessing = std::clamp(level, 0.5f, 2.0f);
}

void Serotonin::signalAnxiety(float level) {
    pImpl->anxiety = std::clamp(level, 0.1f, 1.5f);
}

void Serotonin::signalImpulsivity(float level) {
    pImpl->impulsivity = std::clamp(level, 0.3f, 1.5f);
}

void Serotonin::signalSocialBehavior(float level) {
    pImpl->socialBehavior = std::clamp(level, 0.5f, 2.0f);
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->mood = std::clamp(1.0f + pImpl->level * 0.5f, 0.3f, 2.0f);
    pImpl->rewardProcessing = std::clamp(1.0f + pImpl->level * 0.3f, 0.5f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    return 0.8f + 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Get 5-HT-specific effects
float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getSleepWake() const {
    return pImpl->sleepWake;
}

float Serotonin::getRewardProcessing() const {
    return pImpl->rewardProcessing;
}

float Serotonin::getAnxiety() const {
    return pImpl->anxiety;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

} // namespace nlm