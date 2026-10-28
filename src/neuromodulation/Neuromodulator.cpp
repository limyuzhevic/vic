#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Neuromodulator implementation

Dopamine::Dopamine() : pImpl(new Impl) {
    // Initialize dopamine state
    pImpl->level = 0.0f;
    pImpl->plasticityFactor = 1.0f;
    pImpl->predictionError = 0.0f;
    pImpl->rewardHistory.reserve(100);
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const { return "Dopamine"; }

float Dopamine::getLevel() const { return pImpl->level; }

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, -1.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine affects plasticity based on reward prediction error
    // Positive dopamine (reward) increases plasticity for LTP
    // Negative dopamine (punishment) decreases plasticity or promotes LTD
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level *= 0.95f;  // Natural decay
    
    // Decay plasticity factor
    pImpl->plasticityFactor = std::max(0.1f, pImpl->plasticityFactor * 0.98f);
}

void Dopamine::signalReward(float reward) {
    // Dopamine responds to unexpected rewards with phasic bursts
    float rewardResponse = std::tanh(reward * 2.0f);  // Scale response
    pImpl->level = std::max(pImpl->level, rewardResponse);
    
    // Increase plasticity for learning from rewards
    pImpl->plasticityFactor = std::min(2.0f, pImpl->plasticityFactor + 0.1f);
    
    pImpl->rewardHistory.push_back({pImpl->level, reward});
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine encodes reward prediction error (critic in RL)
    pImpl->predictionError = error;
    pImpl->level = error;  // Direct mapping
    
    // Update plasticity factor based on prediction error
    if (error > 0.1f) {
        // Positive prediction error (better than expected) -> increase plasticity
        pImpl->plasticityFactor = std::min(2.0f, pImpl->plasticityFactor + 0.05f);
    } else if (error < -0.1f) {
        // Negative prediction error (worse than expected) -> decrease plasticity
        pImpl->plasticityFactor = std::max(0.1f, pImpl->plasticityFactor - 0.05f);
    }
}

// Acetylcholine implementation

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    pImpl->level = 0.0f;
    pImpl->attentionBoost = 1.0f;
    pImpl->memoryConsolidation = 0.0f;
}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine neuromodulator initialized");
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay acetylcholine level
    pImpl->level *= 0.9f;
    pImpl->attentionBoost = std::max(1.0f, pImpl->attentionBoost * 0.95f);
    pImpl->memoryConsolidation *= 0.9f;
}

void Acetylcholine::enhanceAttention(float salience) {
    // ACh responds to novel stimuli and enhances attention
    pImpl->level = std::min(1.0f, pImpl->level + salience * 0.3f);
    pImpl->attentionBoost = std::min(2.0f, pImpl->attentionBoost + salience * 0.2f);
}

void Acetylcholine::triggerMemoryConsolidation() {
    // ACh facilitates memory consolidation during awake replay
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.5f);
}

// Norepinephrine implementation

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    pImpl->level = 0.0f;
    pImpl->arousal = 1.0f;
    pImpl->vigilance = 1.0f;
}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE modulates arousal and vigilance
    pImpl->level *= 0.9f;
    pImpl->arousal = std::max(1.0f, pImpl->arousal * 0.95f);
    pImpl->vigilance = std::max(1.0f, pImpl->vigilance * 0.95f);
}

void Norepinephrine::respondToStress(float stressLevel) {
    // NE increases arousal in response to stress/arousal
    pImpl->level = std::min(1.0f, pImpl->level + stressLevel * 0.4f);
    pImpl->arousal = std::min(3.0f, pImpl->arousal + stressLevel * 0.3f);
}

void Norepinephrine::increaseVigilance(float stimulusIntensity) {
    // NE increases vigilance in response to salient stimuli
    pImpl->vigilance = std::min(3.0f, pImpl->vigilance + stimulusIntensity * 0.25f);
}

// Serotonin implementation

Serotonin::Serotonin() : pImpl(new Impl) {
    pImpl->level = 0.0f;
    pImpl->mood = 1.0f;
    pImpl->impulsivity = 1.0f;
}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin neuromodulator initialized");
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin modulates mood and behavior
    pImpl->level *= 0.9f;
    pImpl->mood = std::max(0.1f, pImpl->mood * 0.95f);
    pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity * 0.95f);
}

void Serotonin::respondToPleasure(float pleasureLevel) {
    // 5-HT responds to rewarding stimuli
    pImpl->level = std::min(1.0f, pImpl->level + pleasureLevel * 0.3f);
    pImpl->mood = std::min(2.0f, pImpl->mood + pleasureLevel * 0.2f);
}

void Serotonin::reduceImpulsivity(float controlStrength) {
    // 5-HT reduces impulsivity and promotes behavioral control
    pImpl->impulsivity = std::max(0.1f, pImpl->impulsivity - controlStrength * 0.4f);
}

// Composite neuromodulation implementation

class CompositeNeuromodulation {
public:
    CompositeNeuromodulation() = default;
    
    void addNeuromodulator(Neuromodulator* modulator) {
        modulators.push_back(modulator);
    }
    
    void updateAll(TimestepDuration dt) {
        float totalPlasticityFactor = 1.0f;
        float totalAttentionBoost = 1.0f;
        float totalArousal = 1.0f;
        float totalMood = 1.0f;
        
        for (auto* modulator : modulators) {
            modulator->update(dt);
            
            // Compound effects
            if (auto* dopamine = dynamic_cast<Dopamine*>(modulator)) {
                totalPlasticityFactor *= dopamine->getPlasticityFactor();
            } else if (auto* ach = dynamic_cast<Acetylcholine*>(modulator)) {
                totalAttentionBoost *= ach->getAttentionBoost();
            } else if (auto* ne = dynamic_cast<Norepinephrine*>(modulator)) {
                totalArousal *= ne->getArousal();
            } else if (auto* serotonin = dynamic_cast<Serotonin*>(modulator)) {
                totalMood *= serotonin->getMood();
            }
        }
        
        // Store compound effects
        compoundEffects.plasticityFactor = totalPlasticityFactor;
        compoundEffects.attentionBoost = totalAttentionBoost;
        compoundEffects.arousal = totalArousal;
        compoundEffects.mood = totalMood;
    }
    
    const CompoundEffects& getCompoundEffects() const { return compoundEffects; }
    
private:
    struct CompoundEffects {
        float plasticityFactor = 1.0f;
        float attentionBoost = 1.0f;
        float arousal = 1.0f;
        float mood = 1.0f;
    };
    
    std::vector<Neuromodulator*> modulators;
    CompoundEffects compoundEffects;
};

} // namespace nlm