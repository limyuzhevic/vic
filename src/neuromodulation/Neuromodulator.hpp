#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real neuromodulation effects: biologically-inspired neural dynamics
// Neuromodulator: Abstract base for neuromodulatory signals
// Real neuromodulation effects: biologically-inspired neural dynamics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Real neuromodulation: integrates with neural circuitry
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with biological dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator's target neuron types
    virtual std::vector<NeuronType> getTargetNeuronTypes() const = 0;
    
    // Get neuromodulator's effect strength
    virtual float getEffectStrength() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real dopamine: dynamic neuromodulator with prediction error signaling

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::vector<NeuronType> getTargetNeuronTypes() const override;
    float getEffectStrength() const override;
    
    // Reward signaling: spike-timing dependent reward detection
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalPredictionError(float predictionError);
    
    // Get dopamine's prediction error component
    float getPredictionError() const;
    
    // Get dopamine's reward sensitivity
    float getRewardSensitivity() const;
    
    // Get dopamine's temporal difference
    float getTemporalDifference() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Real ACh: cholinergic modulation of attention and plasticity

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::vector<NeuronType> getTargetNeuronTypes() const override;
    float getEffectStrength() const override;
    
    // Attention modulation: enhances feature selectivity
    void enhanceAttention(float strength);
    
    // Memory modulation: facilitates consolidation
    void facilitateMemoryConsolidation();
    
    // Get attention gain
    float getAttentionGain() const;
    
    // Get memory modulation strength
    float getMemoryModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real NE: noradrenergic system for arousal and learning modulation

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::vector<NeuronType> getTargetNeuronTypes() const override;
    float getEffectStrength() const override;
    
    // Arousal modulation: increases signal-to-noise ratio
    void increaseArousal(float level);
    
    // Vigilance modulation: maintains focused attention
    void increaseVigilance(float level);
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Get vigilance level
    float getVigilanceLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real 5-HT: serotonergic system for mood and behavioral regulation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    std::vector<NeuronType> getTargetNeuronTypes() const override;
    float getEffectStrength() const override;
    
    // Mood modulation: regulates behavioral state
    void modulateMood(float moodLevel);
    
    // Impulse control: modulates decision-making
    void regulateImpulsivity(float controlLevel);
    
    // Social behavior modulation: influences social decisions
    void modulateSocialBehavior(float socialFactor);
    
    // Get mood level
    float getMoodLevel() const;
    
    // Get impulse control
    float getImpulseControl() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
