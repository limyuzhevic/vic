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
    
    // Get neuromodulator-specific properties
    virtual float getLearningRate() const = 0;
    virtual float getTemporalDifference() const = 0;
    
    // Get plasticity factor for specific neuron
    virtual float getPlasticityFactorFor(NeuronId neuronId) const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
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
    
    // Neuromodulator interface
    float getLearningRate() const override;
    float getTemporalDifference() const override;
    float getPlasticityFactorFor(NeuronId neuronId) const override;
    
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
    
    // Neuromodulator interface
    float getLearningRate() const override { return 0.0f; }
    float getTemporalDifference() const override { return 0.0f; }
    float getPlasticityFactorFor(NeuronId neuronId) const override { return 1.0f; }
    
    // Acetylcholine-specific properties
    float getAttentionLevel() const { return 0.5f; }
    void setAttentionLevel(float level) {}
    float getFocusDepth() const { return 0.0f; }
    void enhanceFocus(float duration) {}
    float getModulationStrength() const { return 1.0f; }
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
    
    // Neuromodulator interface
    float getLearningRate() const override { return 0.0f; }
    float getTemporalDifference() const override { return 0.0f; }
    float getPlasticityFactorFor(NeuronId neuronId) const override { return 1.0f; }
    
    // Norepinephrine-specific properties
    float getArousalLevel() const { return 0.5f; }
    float getVigilance() const { return 0.5f; }
    void increaseArousal(float amount) {}
    void decreaseArousal(float amount) {}
    void setVigilanceThreshold(float threshold) {}
    float getArousalFor(const NeuronId& neuronId) const { return 0.0f; }
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
    
    // Neuromodulator interface
    float getLearningRate() const override { return 0.0f; }
    float getTemporalDifference() const override { return 0.0f; }
    float getPlasticityFactorFor(NeuronId neuronId) const override { return 1.0f; }
    
    // Serotonin-specific properties
    float getMoodLevel() const { return 0.5f; }
    float getImpulsivity() const { return 0.5f; }
    void modulateMood(float moodAdjustment) {}
    void setImpulsivity(float impulsivity) {}
    float getSerotoninFor(const NeuronId& neuronId) const { return 0.0f; }
};

} // namespace nlm
