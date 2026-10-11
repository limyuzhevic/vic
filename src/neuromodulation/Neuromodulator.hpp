#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Neuromodulators are diffuse chemical signals that affect many neurons simultaneously
// They modulate neural excitability, plasticity, and learning processes
// This abstract base class defines the interface for all neuromodulators
// Real implementations will be developed in Phase 2 with biologically-inspired dynamics
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get neuromodulator name
    // Returns: String identifier for the neuromodulator (e.g., "Dopamine", "ACh")
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    // Returns: Current neuromodulator concentration (normalized, typically [0.0, 1.0])
    // Represents the strength of neuromodulatory influence
    virtual float getLevel() const = 0;
    
    // Set neuromodulator level/concentration
    // Parameters: level - New concentration level (should be clamped to valid range)
    virtual void setLevel(float level) = 0;
    
    // Get neuromodulatory effect on plasticity
    // Returns: Multiplicative factor applied to plasticity rules (e.g., 0.5 = 50% reduction)
    // Factors > 1.0 enhance plasticity, factors < 1.0 suppress it
    // TODO PHASE 2: Implement real modulation based on neuromodulator type and dynamics
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state over time
    // Parameters: dt - Timestep duration (seconds)
    // Updates internal dynamics, integrates inputs, and produces outputs
    // Real implementations will have differential equations for neuromodulator dynamics
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward prediction error signal - primary reinforcement learning signal
// Key functions:
// - Mediates reward prediction error (RPE) signaling
// - Influences synaptic strengthening of reward-predictive connections
// - Modulates exploration vs. exploitation behavior
// - Affects working memory consolidation
// Biological basis: VTA to prefrontal cortex projections
// 
// Dopamine implementation placeholder - Phase 2 will implement real dynamics
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal receipt of external reward
    // Parameters: reward - Magnitude of reward received (typically 0.0 to 1.0)
    // Updates internal reward prediction error calculations
    // Triggers synaptic strengthening for reward-predictive pathways
    void signalReward(float reward);
    
    // Signal reward prediction error
    // Parameters: error - Difference between expected and actual reward
    // Positive error: better than expected (learning boost)
    // Negative error: worse than expected (learning suppression)
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine (ACh): Attention modulation and memory consolidation signal
// Key functions:
// - Enhances attention to salient stimuli
// - Modulates working memory encoding and consolidation
// - Reduces distractibility during learning
// - Promotes exploratory behavior
// Biological basis: Basal forebrain to cortex projections
// 
// Acetylcholine implementation placeholder - Phase 2 will implement real dynamics
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine (NE): Arousal, vigilance, and stress response signal
// Key functions:
// - Increases neural arousal and alertness
// - Enhances encoding of salient/novel events
// - Reduces reaction time for important stimuli
// - Modulates working memory capacity
// Biological basis: Locus coeruleus to forebrain projections
// 
// Norepinephrine implementation placeholder - Phase 2 will implement real dynamics
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin (5-HT): Mood regulation, impulsivity, and social behavior signal
// Key functions:
// - Regulates mood and emotional state
// - Modulates impulsivity and decision-making
// - Influences social behavior and aggression
// - Affects sleep, appetite, and other homeostatic functions
// Biological basis: Raphe nuclei to forebrain projections
// 
// Serotonin implementation placeholder - Phase 2 will implement real dynamics
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
