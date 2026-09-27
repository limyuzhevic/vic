#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Dopamine Implementation
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float predictionError;
    float lastReward;
    double timeSinceLastEvent;
    
    // Dopamine dynamics parameters
    float tonicLevel;           // Background dopamine level
    float phasicGain;           // Response magnitude to rewards
    float predictionErrorGain;  // Sensitivity to prediction errors
    floatadaptationRate;        // Rate of adaptation to changing rewards
    
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), 
          decayRate(0.1f), releaseRate(1.0f), learningRate(0.01f),
          predictionError(0.0f), lastReward(0.0f), timeSinceLastEvent(0.0),
          tonicLevel(0.05f), phasicGain(2.0f), predictionErrorGain(1.5f),
          adaptationRate(0.001f) {}
};

Dopamine::Dopamine() : pImpl(new Impl), currentBaseline_(0.05f), learningRate_(0.01f) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Based on: higher dopamine increases plasticity up to a point
    float normalizedLevel = (getLevel() - pImpl->baseline) / 
                           std::max(0.01f, pImpl->peak - pImpl->baseline);
    normalizedLevel = std::clamp(normalizedLevel, 0.0f, 1.0f);
    
    // Inverted U-shaped relationship (optimal plasticity at moderate levels)
    float optimalLevel = 0.5f;
    float plasticityFactor = std::exp(-std::pow(normalizedLevel - optimalLevel, 2) / 0.1f);
    
    // Modulate with prediction error
    float errorModulation = 1.0f + pImpl->predictionErrorGain * pImpl->predictionError;
    errorModulation = std::clamp(errorModulation, 0.1f, 2.0f);
    
    return plasticityFactor * errorModulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on temporal difference learning
    timeSinceLastEvent += dt;
    
    // Decay towards baseline with time constant
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Adaptive baseline based on recent rewards
    float rewardHistory = 0.9f * pImpl->lastReward + 0.1f * pImpl->level;
    pImpl->baseline = pImpl->tonicLevel + pImpl->adaptationRate * (rewardHistory - pImpl->tonicLevel);
    pImpl->baseline = std::clamp(pImpl->baseline, 0.01f, 0.2f);
    
    // Reset prediction error over time
    pImpl->predictionError *= std::exp(-static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst proportional to unexpected reward
    float phasicIncrease = pImpl->phasicGain * std::max(0.0f, reward - pImpl->lastReward);
    phasicIncrease *= (1.0f - pImpl->level); // Diminishing returns
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicIncrease);
    pImpl->lastReward = reward;
    timeSinceLastEvent = 0.0;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction errors (TD learning)
    pImpl->predictionError = std::clamp(error, -1.0f, 1.0f);
    
    // Credit assignment: prediction error modulates learning
    float errorModulation = 1.0f + pImpl->predictionErrorGain * error;
    errorModulation = std::clamp(errorModulation, 0.0f, 2.0f);
    
    // Apply error-based learning
    float learningUpdate = learningRate_ * error;
    pImpl->level += learningUpdate;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::setContextBaseline(float baseline) {
    currentBaseline_ = std::clamp(baseline, 0.01f, 0.5f);
    pImpl->tonicLevel = currentBaseline_;
}

// Acetylcholine Implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention and memory parameters
    float attentionGain;
    float memoryConsolidationRate;
    float cholinergicTone;
    
    Impl() 
        : level(0.0f), baseline(0.02f), peak(1.0f),
          decayRate(0.05f), releaseRate(2.0f),
          attentionGain(1.5f), memoryConsolidationRate(0.01f),
          cholinergicTone(0.1f) {}
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity based on attentional state
    float attentionModulation = 1.0f + pImpl->attentionGain * getLevel();
    
    // ACh enhances synaptic consolidation and working memory
    float memoryModulation = 1.0f + pImpl->memoryConsolidationRate * getLevel() * 10.0f;
    
    return std::min(2.0f, attentionModulation * memoryModulation);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay and update cholinergic tone
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Update cholinergic tone based on attention
    pImpl->cholinergicTone = std::min(1.0f, pImpl->cholinergicTone + 0.01f * static_cast<float>(dt));
}

void Acetylcholine::updateAttention(float attentionStrength, TimestepDuration dt) {
    if (attentionStrength > 0.0f) {
        float phasicIncrease = pImpl->releaseRate * attentionStrength;
        pImpl->level = std::min(pImpl->peak, pImpl->level + phasicIncrease);
    }
}

float Acetylcholine::getAttentionLevel() const {
    return std::min(1.0f, getLevel() * 2.0f);
}

float Acetylcholine::getMemoryModulationFactor() const {
    return 1.0f + pImpl->memoryConsolidationRate * getLevel() * 5.0f;
}

// Norepinephrine Implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal parameters
    float arousalGain;
    float stressSensitivity;
    float vigilanceLevel;
    
    float lastArousalTime;
    float stressAccumulation;
    
    Impl() 
        : level(0.0f), baseline(0.01f), peak(2.0f),
          decayRate(0.02f), releaseRate(3.0f),
          arousalGain(2.0f), stressSensitivity(0.8f),
          vigilanceLevel(0.5f),
          lastArousalTime(0.0f), stressAccumulation(0.0f) {}
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and stress
    float arousalModulation = 1.0f + pImpl->arousalGain * getLevel();
    
    // High stress (very high NE) reduces plasticity
    float stressModulation = 1.0f - pImpl->stressSensitivity * 
                            std::max(0.0f, getLevel() - 1.5f);
    stressModulation = std::clamp(stressModulation, 0.1f, 2.0f);
    
    return arousalModulation * stressModulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay arousal and update stress
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Stress accumulates over time with high arousal
    if (getLevel() > 0.5f) {
        pImpl->stressAccumulation += static_cast<float>(dt) * getLevel() * 0.01f;
    }
    
    // Gradual stress recovery
    pImpl->stressAccumulation *= std::exp(-static_cast<float>(dt) * 0.001f);
}

void Norepinephrine::signalArousalEvent() {
    float phasicIncrease = pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicIncrease);
    pImpl->lastArousalTime = 0.0f;
}

void Norepinephrine::reduceArousal(TimestepDuration dt) {
    pImpl->level *= std::exp(-static_cast<float>(dt) * 0.1f);
    pImpl->lastArousalTime += dt;
}

float Norepinephrine::getArousalLevel() const {
    return std::min(1.0f, getLevel() / 1.5f);
}

float Norepinephrine::getStressLevel() const {
    return std::min(1.0f, pImpl->stressAccumulation);
}

// Serotonin Implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and social parameters
    float moodGain;
    float socialMotivation;
    float impulsivityGain;
    
    float currentMood;
    float socialValueHistory;
    
    Impl() 
        : level(0.0f), baseline(0.02f), peak(2.0f),
          decayRate(0.03f), releaseRate(1.5f),
          moodGain(1.0f), socialMotivation(1.0f),
          impulsivityGain(0.5f),
          currentMood(0.5f), socialValueHistory(0.0f) {}
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and social state
    float moodModulation = 1.0f + pImpl->moodGain * (pImpl->currentMood - 0.5f) * 2.0f;
    
    // Social motivation enhances plasticity for social learning
    float socialModulation = 1.0f + pImpl->socialMotivation * pImpl->socialValueHistory;
    
    // Serotonin reduces impulsivity and stabilizes learning
    float impulsivityModulation = 1.0f - pImpl->impulsivityGain * 
                                 std::max(0.0f, getLevel() - 1.0f);
    impulsivityModulation = std::clamp(impulsivityModulation, 0.2f, 1.5f);
    
    return moodModulation * socialModulation * impulsivityModulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Gradual decay and mood recovery
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Mood recovery towards neutral (0.5)
    pImpl->currentMood = 0.5f * (1.0f - std::exp(-static_cast<float>(dt) * 0.01f)) +
                        pImpl->currentMood * std::exp(-static_cast<float>(dt) * 0.01f);
}

void Serotonin::updateMood(float newMood) {
    pImpl->currentMood = std::clamp(newMood, 0.0f, 1.0f);
    
    // Update serotonin level based on mood change
    float moodEffect = (pImpl->currentMood - 0.5f) * pImpl->moodGain;
    pImpl->level += moodEffect;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Serotonin::signalSocialInteraction(float socialValue) {
    pImpl->socialValueHistory = socialValue;
    
    // Social positive reinforcement increases serotonin
    if (socialValue > 0.5f) {
        float socialBoost = pImpl->releaseRate * socialValue * 0.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + socialBoost);
    }
}

float Serotonin::getMoodLevel() const {
    return pImpl->currentMood;
}

float Serotonin::getImpulsivityLevel() const {
    // Impulsivity is inversely related to serotonin level
    float normalizedLevel = std::min(1.0f, getLevel() / 2.0f);
    return 1.0f - normalizedLevel * (1.0f - 0.2f); // Range: 0.2 to 1.0
}

// NeuromodulationSystem Implementation
struct NeuromodulationSystem::Impl {
    std::unordered_map<std::string, std::unique_ptr<Neuromodulator>> modulators;
};

NeuromodulationSystem::NeuromodulationSystem() : pImpl(std::make_unique<Impl>()) {}

NeuromodulationSystem::~NeuromodulationSystem() = default;

void NeuromodulationSystem::addNeuromodulator(std::unique_ptr<Neuromodulator> modulator) {
    if (modulator) {
        pImpl->modulators[modulator->getName()] = std::move(modulator);
    }
}

Neuromodulator* NeuromodulationSystem::getNeuromodulator(const std::string& name) {
    auto it = pImpl->modulators.find(name);
    return (it != pImpl->modulators.end()) ? it->second.get() : nullptr;
}

void NeuromodulationSystem::update(TimestepDuration dt) {
    for (auto& [name, modulator] : pImpl->modulators) {
        modulator->update(dt);
    }
}

float NeuromodulationSystem::getCombinedPlasticityFactor() const {
    if (pImpl->modulators.empty()) return 1.0f;
    
    float totalFactor = 1.0f;
    for (const auto& [name, modulator] : pImpl->modulators) {
        totalFactor *= modulator->getPlasticityFactor();
    }
    
    return totalFactor;
}

float NeuromodulationSystem::getOverallModulationLevel() const {
    if (pImpl->modulators.empty()) return 0.0f;
    
    float totalLevel = 0.0f;
    for (const auto& [name, modulator] : pImpl->modulators) {
        totalLevel += modulator->getLevel();
    }
    
    return totalLevel / pImpl->modulators.size();
}

void NeuromodulationSystem::reset() {
    for (auto& [name, modulator] : pImpl->modulators) {
        modulator->setLevel(modulator->getBaselineLevel());
    }
}

} // namespace nlm
