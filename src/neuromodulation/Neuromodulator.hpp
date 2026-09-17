#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Higher factor increases plasticity, lower decreases it
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Check if modulator should be active based on current brain state
    virtual bool isActive() const { return getLevel() > 0.01f; }
    
    // Get modulator type for categorization
    virtual NeuromodulatorType getType() const = 0;
    
    // Apply effect to neuron membrane potential
    virtual void applyToNeuron(Neuron* neuron, float factor) = 0;
    
    // Apply effect to synaptic plasticity
    virtual void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) = 0;
    
protected:
    Neuromodulator() = default;
};

// Neuromodulator types for categorization
enum class NeuromodulatorType {
    Dopamine,
    Acetylcholine,
    Norepinephrine,
    Serotonin,
    Novelty,
    Curiosity,
    PredictionError
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void applyToNeuron(Neuron* neuron, float factor) override;
    void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) override;
    
    // Reward signaling methods
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Dopamine-specific state
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    
    float getBaseline() const;
    float getPeak() const;
    float getDecayRate() const;
    float getReleaseRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void applyToNeuron(Neuron* neuron, float factor) override;
    void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) override;
    
    // ACh-specific functions
    void enhanceAttention();
    void facilitateMemoryEncoding();
    void modulateSensoryProcessing(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void applyToNeuron(Neuron* neuron, float factor) override;
    void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) override;
    
    // NE-specific functions
    void increaseArousal();
    void enhanceFocus();
    void modulateLiking();
    void improveWorkingMemory(float capacityIncrease);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void applyToNeuron(Neuron* neuron, float factor) override;
    void applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) override;
    
    // 5-HT specific functions
    void improveMood();
    void decreaseImpulsivity();
    void modulateSocialBehavior(float sociality);
    void facilitateSensoryGating();
    
private:
    struct Impl;
    Impl* pImpl;
};
