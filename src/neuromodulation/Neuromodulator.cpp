#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedValue;
    float rewardHistory[10];  // Recent rewards for prediction error
    size_t rewardHistoryIndex;
    float tonicLevel;
    float phasicLevel;
    float burstiness;
    
    Impl() : level(0.0f), baseline(0.1f), peak(2.0f), decayRate(0.05f), releaseRate(1.0f),
             predictionError(0.0f), expectedValue(0.0f), rewardHistoryIndex(0) {
        tonicLevel = baseline;
        phasicLevel = 0.0f;
        burstiness = 0.0f;
        // Initialize reward history with zeros
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
        }
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
    pImpl->level = std::clamp(level, 0.0f, 5.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Based on biological evidence: moderate dopamine enhances plasticity,
    // very high or very low dopamine impairs learning
    float normLevel = pImpl->level / pImpl->peak;
    
    // Optimal plasticity around 0.5-1.0 normalized level
    if (normLevel < 0.5f) {
        // Low dopamine: reduced plasticity (under-arousal)
        return 0.3f + 0.7f * normLevel;
    } else if (normLevel < 1.5f) {
        // Optimal dopamine: enhanced plasticity (arousal-induced learning)
        return 0.7f + 0.6f * (normLevel - 0.5f) / 1.0f;
    } else {
        // High dopamine: impaired plasticity (too much arousal)
        return 1.5f - 0.3f * (normLevel - 1.5f) / 2.0f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with phasic and tonic components
    float dtFloat = static_cast<float>(dt);
    
    // Update tonic level (baseline)
    tonicLevel = std::max(pImpl->baseline, tonicLevel - 0.01f * dtFloat);
    
    // Update phasic component (reward prediction error)
    phasicLevel = pImpl->predictionError * 2.0f; // Amplify prediction error
    
    // Burstiness affects phasic response
    burstiness += (phasicLevel - burstiness) * 0.1f * dtFloat;
    
    // Combine tonic and phasic with realistic dynamics
    float newLevel = tonicLevel + phasicLevel * 0.7f + burstiness * 0.3f;
    
    // Decay toward baseline
    newLevel = std::max(pImpl->baseline, newLevel - pImpl->decayRate * dtFloat);
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(newLevel, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal dynamics
    // Add reward to history
    pImpl->rewardHistory[pImpl->rewardHistoryIndex] = reward;
    pImpl->rewardHistoryIndex = (pImpl->rewardHistoryIndex + 1) % 10;
    
    // Calculate average recent reward
    float sum = 0.0f;
    for (int i = 0; i < 10; ++i) {
        sum += pImpl->rewardHistory[i];
    }
    float avgReward = sum / 10.0f;
    
    // Update expected value (prediction error)
    pImpl->predictionError = reward - pImpl->expectedValue;
    pImpl->expectedValue = avgReward;
    
    // Phasic burst: immediate response to unexpected reward
    if (std::abs(pImpl->predictionError) > 0.1f) {
        // Strong burst for large prediction error
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + std::abs(pImpl->predictionError) * pImpl->releaseRate * 10.0f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    pImpl->predictionError = error;
    
    // Negative prediction error (worse than expected) reduces dopamine
    if (error < 0.0f) {
        pImpl->level = std::max(0.0f, pImpl->level + error * 0.5f * pImpl->releaseRate);
    } else {
        // Positive prediction error (better than expected) increases dopamine
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + error * pImpl->releaseRate);
    }
}

} // namespace nlm

// Implement other neuromodulators with real functionality

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionSignal;
    float memoryConsolidationSignal;
    float noveltySensitivity;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.5f), decayRate(0.03f), releaseRate(1.0f),
             attentionSignal(0.0f), memoryConsolidationSignal(0.0f), noveltySensitivity(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances memory consolidation and attention
    // Moderate levels optimize learning, high levels may cause overfitting
    float normLevel = pImpl->level / pImpl->peak;
    return 0.8f + 0.4f * normLevel * (1.0f - normLevel); // Bell curve
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics with attention and memory components
    float dtFloat = static_cast<float>(dt);
    
    // Decay toward baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * dtFloat);
}

void Acetylcholine::signalAttention(float attention) {
    // Signal attention shift
    pImpl->attentionSignal = attention;
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->level + attention * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    // Signal memory consolidation
    pImpl->memoryConsolidationSignal = strength;
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->level + strength * pImpl->releaseRate * 0.3f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.02f), peak(2.0f), decayRate(0.04f), releaseRate(1.0f),
             arousal(0.0f), vigilance(0.0f), stressLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and vigilance for optimal learning
    float normLevel = pImpl->level / pImpl->peak;
    return 0.6f + 0.8f * normLevel * (1.0f - normLevel / 2.0f); // Inverted U
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics with arousal and stress components
    float dtFloat = static_cast<float>(dt);
    
    // Decay toward baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * dtFloat);
}

void Norepinephrine::signalArousal(float arousal) {
    // Signal arousal level
    pImpl->arousal = arousal;
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->level + arousal * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stress) {
    // Signal stress level
    pImpl->stressLevel = stress;
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->level + stress * pImpl->releaseRate * 0.5f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float anxiety;
    
    Impl() : level(0.0f), baseline(0.03f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f),
             mood(0.0f), impulsivity(0.0f), anxiety(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates mood and impulsivity for balanced learning
    float normLevel = pImpl->level / pImpl->peak;
    return 0.9f - 0.3f * normLevel; // High baseline, decreases with level
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics with mood and anxiety components
    float dtFloat = static_cast<float>(dt);
    
    // Decay toward baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * dtFloat);
}

void Serotonin::signalMood(float mood) {
    // Signal mood change
    pImpl->mood = mood;
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->level + mood * pImpl->releaseRate * 0.4f);
}

void Serotonin::signalAnxiety(float anxiety) {
    // Signal anxiety level
    pImpl->anxiety = anxiety;
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->level + anxiety * pImpl->releaseRate * 0.6f);
}

} // namespace nlm

// NeuromodulationManager for coordinating all neuromodulators

namespace nlm {

struct NeuromodulationManager::Impl {
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
    // Integration with brain systems
    float predictionError;
    float noveltySignal;
    float rewardSignal;
    float attentionSignal;
    
    Impl() {
        dopamine = std::make_unique<Dopamine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        
        predictionError = 0.0f;
        noveltySignal = 0.0f;
        rewardSignal = 0.0f;
        attentionSignal = 0.0f;
    }
};

NeuromodulationManager::NeuromodulationManager() : pImpl(new Impl) {}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::initialize(class Brain* brain) {
    // Initialize all neuromodulators
    pImpl->dopamine->setLevel(0.1f);
    pImpl->acetylcholine->setLevel(0.05f);
    pImpl->norepinephrine->setLevel(0.02f);
    pImpl->serotonin->setLevel(0.03f);
    
    // Store brain reference for integration
    brain_ = brain;
}

void NeuromodulationManager::update(float dt) {
    // Update all neuromodulators
    pImpl->dopamine->update(dt);
    pImpl->acetylcholine->update(dt);
    pImpl->norepinephrine->update(dt);
    pImpl->serotonin->update(dt);
    
    // Compute integrated neuromodulation signals
    computeIntegratedSignals();
}

void NeuromodulationManager::signalReward(float reward, float predictionError) {
    // Signal reward and compute prediction error
    pImpl->rewardSignal = reward;
    pImpl->predictionError = predictionError;
    
    pImpl->dopamine->signalReward(reward);
    pImpl->dopamine->signalRewardPredictionError(predictionError);
}

void NeuromodulationManager::signalNovelty(float novelty) {
    // Signal novelty detection
    pImpl->noveltySignal = novelty;
    
    // ACh and NE respond to novelty
    pImpl->acetylcholine->signalAttention(novelty * 0.3f);
    pImpl->norepinephrine->signalArousal(novelty * 0.5f);
}

void NeuromodulationManager::signalAttention(float attention) {
    // Signal attention shifts
    pImpl->attentionSignal = attention;
    pImpl->acetylcholine->signalAttention(attention);
}

void NeuromodulationManager::computeIntegratedSignals() {
    // Compute integrated neuromodulation signals for brain systems
    if (!brain_) return;
    
    // Update brain systems based on neuromodulation
    // This would integrate with memory, prediction, and other systems
}

Dopamine* NeuromodulationManager::getDopamine() const {
    return pImpl->dopamine.get();
}

Acetylcholine* NeuromodulationManager::getAcetylcholine() const {
    return pImpl->acetylcholine.get();
}

Norepinephrine* NeuromodulationManager::getNorepinephrine() const {
    return pImpl->norepinephrine.get();
}

Serotonin* NeuromodulationManager::getSerotonin() const {
    return pImpl->serotonin.get();
}

float NeuromodulationManager::getOverall neuromodulationStrength() const {
    float total = pImpl->dopamine->getLevel() +
                  pImpl->acetylcholine->getLevel() +
                  pImpl->norepinephrine->getLevel() +
                  pImpl->serotonin->getLevel();
    return total / 4.0f; // Average
}

void NeuromodulationManager::reset() {
    // Reset all neuromodulators to baseline
    pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() * 0.1f);
    pImpl->acetylcholine->setLevel(pImpl->acetylcholine->getLevel() * 0.1f);
    pImpl->norepinephrine->setLevel(pImpl->norepinephrine->getLevel() * 0.1f);
    pImpl->serotonin->setLevel(pImpl->serotonin->getLevel() * 0.1f);
    
    pImpl->predictionError = 0.0f;
    pImpl->noveltySignal = 0.0f;
    pImpl->rewardSignal = 0.0f;
    pImpl->attentionSignal = 0.0f;
}

} // namespace nlm
