#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced Phase 2 implementation with full biological fidelity

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Phase 2: Enhanced neuromodulation with multiple targets
    virtual float getPlasticityFactor() const = 0;
    virtual void applyToPlasticity(float& stdpLTP, float& stdpLTD,
                                   float& hebbianLearning, float& structuralPlasticity) const = 0;
    
    // Update neuromodulator state based on brain activity
    virtual void update(TimestepDuration dt, float targetActivity = 0.0f,
                       float reward = 0.0f, float novelty = 0.0f,
                       float predictionError = 0.0f) = 0;
    
    // Get attention modulation effect
    virtual float getAttentionModulation() const { return 1.0f; }
    
    // Get memory consolidation effect
    virtual float getMemoryConsolidationFactor() const { return 1.0f; }
    
    // Get arousal level
    virtual float getArousalLevel() const { return 1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced Phase 2 implementation with proper neuromodulatory effects
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void applyToPlasticity(float& stdpLTP, float& stdpLTD,
                           float& hebbianLearning, float& structuralPlasticity) const override;
    void update(TimestepDuration dt, float targetActivity = 0.0f,
                float reward = 0.0f, float novelty = 0.0f,
                float predictionError = 0.0f) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhanced functions for memory consolidation
    void enhanceMemoryConsolidation();
    void modulateSynapticStrength(float& baselineStrength);
    
    // Learning rate modulation
    float getLearningRate() const;
    void setLearningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced Phase 2 implementation with attention and memory effects
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void applyToPlasticity(float& stdpLTP, float& stdpLTD,
                           float& hebbianLearning, float& structuralPlasticity) const override;
    void update(TimestepDuration dt, float targetActivity = 0.0f,
                float reward = 0.0f, float novelty = 0.0f,
                float predictionError = 0.0f) override;
    
    // Attention modulation
    float getAttentionModulation() const override;
    void setAttentionGain(float gain);
    
    // Memory consolidation effects
    float getMemoryConsolidationFactor() const override;
    void triggerMemoryConsolidation();
    
    // Neurogenesis (concept formation)
    void enhanceConceptFormation();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced Phase 2 implementation with arousal and learning effects
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void applyToPlasticity(float& stdpLTP, float& stdpLTD,
                           float& hebbianLearning, float& structuralPlasticity) const override;
    void update(TimestepDuration dt, float targetActivity = 0.0f,
                float reward = 0.0f, float novelty = 0.0f,
                float predictionError = 0.0f) override;
    
    // Arousal modulation
    float getArousalLevel() const override;
    void setArousalLevel(float arousal);
    
    // Alertness and attention
    void enhanceAlertness();
    
    // Signal detection and error monitoring
    void detectAnomalies(float currentActivity);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced Phase 2 implementation with behavioral regulation effects
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void applyToPlasticity(float& stdpLTP, float& stdpLTD,
                           float& hebbianLearning, float& structuralPlasticity) const override;
    void update(TimestepDuration dt, float targetActivity = 0.0f,
                float reward = 0.0f, float novelty = 0.0f,
                float predictionError = 0.0f) override;
    
    // Behavioral regulation
    void regulateBehavioralResponse(float activity);
    void reduceImpulsivity();
    
    // Social behavior modulation
    void modulateSocialBehavior(bool isSocialInteraction);
    
    // Mood and affect regulation
    void stabilizeMood(float emotionalState);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulator: Type alias for easier usage
using NeuromodulatorPtr = std::unique_ptr<Neuromodulator>;

// NeuromodulatorManager: Manages all neuromodulators and their interactions
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt, float reward = 0.0f,
                   float novelty = 0.0f, float predictionError = 0.0f);
    
    // Apply neuromodulatory effects to plasticity
    void applyNeuromodulatoryPlasticity(float& stdpLTP, float& stdpLTD,
                                        float& hebbianLearning, float& structuralPlasticity) const;
    
    // Get combined attention modulation
    float getCombinedAttentionModulation() const;
    
    // Get combined memory consolidation
    float getCombinedMemoryConsolidation() const;
    
    // Get arousal level
    float getCombinedArousal() const;
    
    // Add neuromodulator
    void addDopamine(std::unique_ptr<Dopamine> dopamine);
    void addAcetylcholine(std::unique_ptr<Acetylcholine> ach);
    void addNorepinephrine(std::unique_ptr<Norepinephrine> ne);
    void addSerotonin(std::unique_ptr<Serotonin> serotonin);
    
    // Get neuromodulator by type
    Dopamine* getDopamine() const;
    Acetylcholine* getAcetylcholine() const;
    Norepinephrine* getNorepinephrine() const;
    Serotonin* getSerotonin() const;
    
    // Signal various neuromodulators
    void signalReward(float reward);
    void signalNovelty(float novelty);
    void signalPredictionError(float error);
    
    // Update based on brain state
    void updateFromBrainState(const class BrainState& state);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Convenience functions
inline float getCombinedArousal(const NeuromodulatorManager& manager) {
    return manager.getCombinedArousal();
}

inline float getCombinedAttention(const NeuromodulatorManager& manager) {
    return manager.getCombinedAttentionModulation();
}

inline float getCombinedMemoryConsolidation(const NeuromodulatorManager& manager) {
    return manager.getCombinedMemoryConsolidation();
}
