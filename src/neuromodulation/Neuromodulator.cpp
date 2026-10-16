// Neuromodulator System Implementation
// Phase 5: Neuromodulation Integration

#include "neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <cmath>

namespace nlm {

// Neuromodulator base implementation
class Neuromodulator {
public:
    Neuromodulator(const std::string& name) : name_(name), level_(0.0f), 
                                              plasticityFactor_(1.0f), minLevel_(0.0f), maxLevel_(1.0f),
                                              decayRate_(0.01f), targetLevel_(0.0f) {}
    
    virtual ~Neuromodulator() = default;
    
    const char* getName() const { return name_.c_str(); }
    std::string getNameString() const { return name_; }
    
    float getLevel() const { return level_; }
    void setLevel(float level) { level_ = std::clamp(level, minLevel_, maxLevel_); }
    
    float getPlasticityFactor() const { return plasticityFactor_; }
    
    void setPlasticityFactor(float factor) { plasticityFactor_ = factor; }
    
    float getMinLevel() const { return minLevel_; }
    void setMinLevel(float level) { minLevel_ = level; }
    
    float getMaxLevel() const { return maxLevel_; }
    void setMaxLevel(float level) { maxLevel_ = level; }
    
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    void setTargetLevel(float level) { targetLevel_ = level; }
    
    virtual void update(TimestepDuration dt) {
        // Decay toward baseline (0.0)
        level_ = std::max(0.0f, level_ * (1.0f - decayRate_ * dt));
        
        // Slowly approach target level
        if (targetLevel_ != level_) {
            float step = (targetLevel_ > level_) ? 
                std::min(0.1f, (targetLevel_ - level_) / dt) : 
                std::max(-0.1f, (targetLevel_ - level_) / dt);
            level_ = std::clamp(level_ + step, minLevel_, maxLevel_);
        }
        
        updatePlasticityFactor();
    }
    
    virtual void reset() {
        level_ = minLevel_;
        targetLevel_ = minLevel_;
    }
    
    virtual void applyToNeuron(float& membranePotential, float& threshold) {
        // Virtual method for applying neuromodulatory effects
        // Subclasses should override
    }
    
    virtual void applyToSynapticWeight(float& weight, float preSpike, float postSpike) {
        // Virtual method for modulating synaptic plasticity
        // Subclasses should override
    }
    
protected:
    virtual void updatePlasticityFactor() {
        plasticityFactor_ = 1.0f + level_ * 0.5f;  // Modulation increases with level
    }
    
    std::string name_;
    float level_;
    float plasticityFactor_;
    float minLevel_;
    float maxLevel_;
    float decayRate_;
    float targetLevel_;
};

// Dopamine implementation
struct Dopamine::Impl {
    Neuromodulator neuromod;
    
    // Reward prediction components
    float lastReward;
    float predictedReward;
    float predictionError;
    
    // Reward tracking
    float rewardAccumulator;
    float rewardHistory[10];  // Circular buffer of recent rewards
    int historyIndex;
    
    // Reinforcement learning components
    float learningRate;
    float explorationRate;
    
    Impl() : neuromod("Dopamine"), lastReward(0.0f), predictedReward(0.0f), 
             predictionError(0.0f), rewardAccumulator(0.0f), historyIndex(0) {
        neuromod.setMinLevel(0.0f);
        neuromod.setMaxLevel(1.0f);
        neuromod.setDecayRate(0.02f);
        neuromod.setTargetLevel(0.0f);
        
        learningRate = 0.01f;
        explorationRate = 0.1f;
        
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
        }
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() { delete pImpl; }

const char* Dopamine::getName() const { return pImpl->neurom.getName(); }
float Dopamine::getLevel() const { return pImpl->neurom.getLevel(); }
void Dopamine::setLevel(float level) { pImpl->neurom.setLevel(level); }
float Dopamine::getPlasticityFactor() const { return pImpl->neurom.getPlasticityFactor(); }
void Dopamine::update(TimestepDuration dt) {
    pImpl->neurom.update(dt);
    
    // Update prediction error based on reward history
    float currentReward = pImpl->rewardAccumulator / 10.0f;
    pImpl->predictionError = currentReward - pImpl->predictedReward;
    
    // Update prediction
    pImpl->predictedReward = 0.9f * pImpl->predictedReward + 0.1f * currentReward;
    
    // Apply prediction error to dopamine level
    float dopamineChange = pImpl->predictionError * pImpl->learningRate;
    setLevel(getLevel() + dopamineChange);
}

void Dopamine::signalReward(float reward) {
    // Store in history
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Update accumulator
    pImpl->rewardAccumulator = 0.0f;
    for (int i = 0; i < 10; ++i) {
        pImpl->rewardAccumulator += pImpl->rewardHistory[i];
    }
    
    // Signal prediction (dopamine surge)
    setLevel(1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    
    // Update learning based on prediction error
    float learningChange = error * pImpl->learningRate;
    setLevel(getLevel() + learningChange * 0.5f);
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    Neuromodulator neuromod;
    
    // Attention components
    float attentionLevel;
    float focusAreas[3];  // Weight on different sensory modalities
    
    // Memory consolidation
    float memoryConsolidation;
    float recentMemoryStrength;
    
    // Learning rate modulation
    float learningRateModulation;
    
    Impl() : neuromod("Acetylcholine"), attentionLevel(0.0f), memoryConsolidation(0.0f),
             recentMemoryStrength(0.0f), learningRateModulation(1.0f) {
        neuromod.setMinLevel(0.0f);
        neuromod.setMaxLevel(1.0f);
        neuromod.setDecayRate(0.01f);
        neuromod.setTargetLevel(0.0f);
        
        focusAreas[0] = 1.0f;  // Vision
        focusAreas[1] = 1.0f;  // Hearing
        focusAreas[2] = 1.0f;  // Internal
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() { delete pImpl; }

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->neurom.update(dt);
    
    // Adjust attention based on ACh level
    pImpl->attentionLevel = getLevel();
    
    // Modulate learning rate
    pImpl->learningRateModulation = 1.0f + getLevel() * 2.0f;
    
    // Enhance memory consolidation
    pImpl->memoryConsolidation = getLevel() * 0.5f;
}

void Acetylcholine::focusOnModality(int modality) {
    if (modality >= 0 && modality < 3) {
        // Shift focus away from other modalities
        for (int i = 0; i < 3; ++i) {
            if (i != modality) {
                pImpl->focusAreas[i] *= 0.5f;
            } else {
                pImpl->focusAreas[i] = 1.5f;
            }
        }
        
        // Increase ACh level to maintain attention
        setLevel(std::min(1.0f, getLevel() + 0.2f));
    }
}

void Acetylcholine::enhanceMemory(const std::vector<float>& memory) {
    if (!memory.empty()) {
        // Strengthen recent memories
        for (auto& val : memory) {
            val = std::min(1.0f, val * (1.0f + pImpl->memoryConsolidation));
        }
        
        pImpl->recentMemoryStrength = pImpl->memoryConsolidation;
    }
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    Neuromodulator neuromod;
    
    // Arousal and vigilance
    float arousalLevel;
    float vigilance;
    
    // Stress and alertness
    float stressLevel;
    float alertness;
    
    // Fight or flight response
    bool fightResponse;
    bool flightResponse;
    
    Impl() : neuromod("Norepinephrine"), arousalLevel(0.0f), vigilance(0.0f),
             stressLevel(0.0f), alertness(0.0f), fightResponse(false), flightResponse(false) {
        neuromod.setMinLevel(0.0f);
        neuromod.setMaxLevel(1.0f);
        neuromod.setDecayRate(0.03f);
        neuromod.setTargetLevel(0.0f);
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() { delete pImpl; }

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->neurom.update(dt);
    
    // Update arousal and alertness
    pImpl->arousalLevel = getLevel() * 1.5f;
    pImpl->alertness = getLevel() * 2.0f;
    
    // Stress increases with norepinephrine
    pImpl->stressLevel = getLevel() * 0.8f;
    
    // High arousal triggers fight/flight responses
    if (getLevel() > 0.7f) {
        // Simulate decision making
        static float stressCounter = 0.0f;
        stressCounter += dt;
        
        if (stressCounter > 100.0f) {
            // Randomly choose fight or flight
            static std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            if (dist(rng) > 0.5f) {
                pImpl->fightResponse = true;
                pImpl->flightResponse = false;
            } else {
                pImpl->fightResponse = false;
                pImpl->flightResponse = true;
            }
            
            stressCounter = 0.0f;
        }
    } else {
        pImpl->fightResponse = false;
        pImpl->flightResponse = false;
    }
}

void Norepinephrine::triggerFightResponse() {
    setLevel(std::min(1.0f, getLevel() + 0.3f));
    pImpl->fightResponse = true;
}

void Norepinephrine::triggerFlightResponse() {
    setLevel(std::min(1.0f, getLevel() + 0.3f));
    pImpl->flightResponse = true;
}

void Norepinephrine::increaseAlertness(float amount) {
    // Increase alertness and arousal
    float newLevel = std::min(1.0f, getLevel() + amount);
    setLevel(newLevel);
    pImpl->alertness = newLevel * 2.0f;
}

// Serotonin implementation
struct Serotonin::Impl {
    Neuromodulator neuromod;
    
    // Mood regulation
    float moodLevel;
    float happiness;
    float sadness;
    
    // Social behavior
    float socialDominance;
    float socialSubmission;
    
    // Impulsivity control
    float impulsivity;
    float behavioralInhibition;
    
    // Reward processing
    float rewardSensitivity;
    float punishmentSensitivity;
    
    Impl() : neuromod("Serotonin"), moodLevel(0.0f), happiness(0.5f), sadness(0.5f),
             socialDominance(0.5f), socialSubmission(0.5f), impulsivity(0.5f),
             behavioralInhibition(0.5f), rewardSensitivity(0.5f), punishmentSensitivity(0.5f) {
        neuromod.setMinLevel(0.0f);
        neuromod.setMaxLevel(1.0f);
        neuromod.setDecayRate(0.015f);
        neuromod.setTargetLevel(0.0f);
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() { delete pImpl; }

void Serotonin::update(TimestepDuration dt) {
    pImpl->neurom.update(dt);
    
    // Update mood based on level
    pImpl->moodLevel = getLevel();
    pImpl->happiness = 0.5f + getLevel() * 0.5f;
    pImpl->sadness = 1.0f - pImpl->happiness;
    
    // Modulate social behavior
    pImpl->socialDominance = 0.5f + getLevel() * 0.4f;
    pImpl->socialSubmission = 1.0f - pImpl->socialDominance;
    
    // Control impulsivity
    pImpl->impulsivity = 0.5f + getLevel() * 0.4f;
    pImpl->behavioralInhibition = 1.0f - pImpl->impulsivity;
    
    // Adjust reward sensitivity
    pImpl->rewardSensitivity = 0.5f + getLevel() * 0.3f;
    pImpl->punishmentSensitivity = 1.0f - pImpl->rewardSensitivity;
}

void Serotonin::increaseHappiness(float amount) {
    // Increase happiness and serotonin level
    pImpl->happiness = std::min(1.0f, pImpl->happiness + amount);
    pImpl->sadness = 1.0f - pImpl->happiness;
    setLevel(std::min(1.0f, getLevel() + amount * 0.5f));
}

void Serotonin::reduceImpulsivity(float amount) {
    // Reduce impulsivity and increase behavioral inhibition
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - amount);
    pImpl->behavioralInhibition = 1.0f - pImpl->impulsivity;
    setLevel(std::min(1.0f, getLevel() + amount * 0.3f));
}

void Serotonin::enhanceSocialBonding() {
    // Increase social connection and reduce aggression
    pImpl->socialDominance *= 0.9f;
    pImpl->socialSubmission = std::min(1.0f, pImpl->socialSubmission + 0.2f);
    setLevel(std::min(1.0f, getLevel() + 0.2f));
}

} // namespace nlm

// Initialize static implementations
namespace nlm {
    Dopamine::Dopamine() : pImpl(new Dopamine::Impl) {}
    Dopamine::~Dopamine() { delete pImpl; }
    
    void Dopamine::signalReward(float reward) {
        pImpl->rewardHistory[pImpl->historyIndex] = reward;
        pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
        
        pImpl->rewardAccumulator = 0.0f;
        for (int i = 0; i < 10; ++i) {
            pImpl->rewardAccumulator += pImpl->rewardHistory[i];
        }
        
        setLevel(1.0f);
    }
    
    void Dopamine::signalRewardPredictionError(float error) {
        pImpl->predictionError = error;
        float learningChange = error * pImpl->learningRate;
        setLevel(getLevel() + learningChange * 0.5f);
    }
    
    void Dopamine::update(TimestepDuration dt) {
        Neuromodulator::update(dt);
        
        float currentReward = pImpl->rewardAccumulator / 10.0f;
        pImpl->predictionError = currentReward - pImpl->predictedReward;
        pImpl->predictedReward = 0.9f * pImpl->predictedReward + 0.1f * currentReward;
        
        float dopamineChange = pImpl->predictionError * pImpl->learningRate;
        setLevel(getLevel() + dopamineChange);
    }
    
    Acetylcholine::Acetylcholine() : pImpl(new Acetylcholine::Impl) {}
    Acetylcholine::~Acetylcholine() { delete pImpl; }
    
    void Acetylcholine::update(TimestepDuration dt) {
        Neuromodulator::update(dt);
        pImpl->attentionLevel = getLevel();
        pImpl->learningRateModulation = 1.0f + getLevel() * 2.0f;
        pImpl->memoryConsolidation = getLevel() * 0.5f;
    }
    
    Norepinephrine::Norepinephrine() : pImpl(new Norepinephrine::Impl) {}
    Norepinephrine::~Norepinephrine() { delete pImpl; }
    
    void Norepinephrine::update(TimestepDuration dt) {
        Neuromodulator::update(dt);
        pImpl->arousalLevel = getLevel() * 1.5f;
        pImpl->alertness = getLevel() * 2.0f;
        pImpl->stressLevel = getLevel() * 0.8f;
    }
    
    Serotonin::Serotonin() : pImpl(new Serotonin::Impl) {}
    Serotonin::~Serotonin() { delete pImpl; }
    
    void Serotonin::update(TimestepDuration dt) {
        Neuromodulator::update(dt);
        pImpl->moodLevel = getLevel();
        pImpl->happiness = 0.5f + getLevel() * 0.5f;
        pImpl->sadness = 1.0f - pImpl->happiness;
    }
}