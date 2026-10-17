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
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator influence on different brain systems
    virtual float getInfluenceOn(float neuronType) const = 0;
    
    virtual void applyEffect(class Neuron* neuron, TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
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
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhanced effects on neural excitability
    void applyToNeuron(class Neuron* neuron, TimestepDuration dt);
    
    // Memory consolidation
    void enhanceMemoryConsolidation();
    
    // Time perception effects
    void modulateTimePerception();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Enhanced attention effects
    void applyToNeuron(class Neuron* neuron, TimestepDuration dt);
    
    // Memory consolidation
    void consolidateMemory(class NeuralWorkingMemory* memory);
    
    // Cognitive enhancement
    void enhanceCognitiveFunctions();
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Enhanced alertness effects
    void applyToNeuron(class Neuron* neuron, TimestepDuration dt);
    
    // Arousal modulation
    void modulateArousal(class Brain* brain, float currentLevel);
    
    // Cognitive enhancement for novelty detection
    void enhanceNoveltyDetection();
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Mood effects on neural activity
    void applyToNeuron(class Neuron* neuron, TimestepDuration dt);
    
    // Social behavior modulation
    void modulateSocialBehavior(class Brain* brain);
    
    // Impulsivity control
    void regulateImpulsivity(class Brain* brain);
};

} // namespace nlm
