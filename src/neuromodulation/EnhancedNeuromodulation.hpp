#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

// Enhanced Neuromodulator: Abstract base with real neuromodulatory effects
// Now implements proper biological neuromodulation dynamics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get influence on specific plasticity rules
    virtual float getSTDPModulation() const = 0;
    virtual float getHebbianModulation() const = 0;
    virtual float getHomeostaticModulation() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Phasic and tonic reward signaling with learning integration
// Implements realistic dopamine dynamics including:
// - Phasic bursts for unexpected rewards
// - Tonic baseline for motivation
// - Prediction error computation
// - Interaction with STDP

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error, float predictedReward);
    
    // Dopamine state
    float getPhasicComponent() const;
    float getTonicComponent() const;
    float getPredictionError() const;
    float getRewardPrediction(float predictedReward) const;
    
    // Learning rate adaptation
    void updateLearningRate(float innovationLevel);
    float getLearningRate() const;
    
    // Reset
    void reset();
    
    // Configuration
    void setPhasicGain(float gain);
    void setTonicDecay(float decay);
    void setLearningRateBase(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory modulation
// Implements ACh effects on:
// - Attentional gating of sensory inputs
// - Memory consolidation
// - Working memory maintenance
// - Cortical excitability

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention mechanisms
    void signalNovelty(float novelty);
    void signalSensoryInput(float inputStrength);
    float getAttentionLevel() const;
    
    // Memory effects
    void initiateMemoryConsolidation();
    void enhanceMemoryTrace(float timeConstant);
    float getMemoryEnhancement() const;
    
    // Configuration
    void setAttentionGain(float gain);
    void setMemoryConsolidationThreshold(float threshold);
    void setBasalLevel(float basal);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance modulation
// Implements NE effects on:
// - Vigilance and attention switching
// - Response vigor and motivation
// - Stress response
// - Working memory capacity

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Vigilance mechanisms
    void updateVigilance(float taskDifficulty, float timeOnTask);
    float getVigilanceLevel() const;
    
    // Response modulation
    void modulateResponse vigor, float baselineEnergy);
    float getResponseVigor() const;
    
    // Stress response
    void signalStress(float stressorIntensity, float duration);
    float getStressLevel() const;
    
    // Configuration
    void setVigilanceGain(float gain);
    void setStressDecay(float decay);
    void setBaselineVigilance(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, social behavior, and behavioral inhibition
// Implements 5-HT effects on:
// - Mood regulation
// - Social behavior and status
// - Behavioral inhibition
// - Sleep-wake cycles

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void updateSocialStatus(float socialRank, float socialSupport);
    float getMoodLevel() const;
    
    // Behavioral inhibition
    void signalConflict(float conflictStrength);
    void inhibitBehavior(float targetStrength);
    float getInhibitionLevel() const;
    
    // Sleep-wake regulation
    void updateSleepPressure(float wakeTime);
    void enterSleep();
    void wakeUp();
    float getSleepPressure() const;
    bool isAsleep() const;
    
    // Configuration
    void setSocialSensitivity(float sensitivity);
    void setInhibitionDecay(float decay);
    void setBaselineSerotonin(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulatory Integration: Coordinator for all neuromodulators
// Manages interactions between different neuromodulators and their effects on plasticity

class NeuromodulatorySystem {
public:
    NeuromodulatorySystem();
    ~NeuromodulatorySystem();
    
    // Get neuromodulator instances
    Dopamine* getDopamine();
    Acetylcholine* getAcetylcholine();
    Norepinephrine* getNorepinephrine();
    Serotonin* getSerotonin();
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
    // Apply integrated neuromodulation to plasticity
    float computeSTDPModulation() const;
    float computeHebbianModulation() const;
    float computeHomeostaticModulation() const;
    
    // Network-level neuromodulation
    void signalReward(float reward, float predictionError);
    void signalNovelty(float novelty);
    void signalStress(float stressLevel);
    void updateVigilance(float difficulty);
    
    // Configuration
    void configureAll(float baselineLevel, float modulationStrength);
    void resetAll();
    
    // Statistics
    float getNetworkExcitability() const;
    float getNetworkStability() const;
    float getLearningReadiness() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

