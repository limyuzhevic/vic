#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

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
    
    // Neuromodulation effects
    void applyToNeuron(Neuron* neuron, TimestepDuration dt) override;
    void applyToSynapse(Synapse* synapse, TimestepDuration dt) override;
    void reset() override;
    float getBaseline() const override;
    void setBaseline(float baseline) override;
    bool isActive() const override;
    
    // Dopamine-specific parameters
    void setDecayRate(float rate);
    float getDecayRate() const;
    void setReleaseRate(float rate);
    float getReleaseRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override { return 1.2f; }
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void enhanceAttention(float attentionStrength);
    
    // Memory effects
    void promoteMemoryConsolidation();
    
    // Neuromodulation effects
    void applyToNeuron(Neuron* neuron, TimestepDuration dt) override;
    void applyToSynapse(Synapse* synapse, TimestepDuration dt) override;
    void reset() override;
    float getBaseline() const override;
    void setBaseline(float baseline) override;
    bool isActive() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override { return 1.3f; }
    void update(TimestepDuration dt) override;
    
    // Arousal effects
    void increaseArousal(float arousalLevel);
    
    // Stress response
    void triggerStressResponse(float stressLevel);
    
    // Neuromodulation effects
    void applyToNeuron(Neuron* neuron, TimestepDuration dt) override;
    void applyToSynapse(Synapse* synapse, TimestepDuration dt) override;
    void reset() override;
    float getBaseline() const override;
    void setBaseline(float baseline) override;
    bool isActive() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override { return 0.8f; }
    void update(TimestepDuration dt) override;
    
    // Mood effects
    void modulateMood(float moodLevel);
    
    // Impulse control
    void regulateImpulseControl(float controlStrength);
    
    // Neuromodulation effects
    void applyToNeuron(Neuron* neuron, TimestepDuration dt) override;
    void applyToSynapse(Synapse* synapse, TimestepDuration dt) override;
    void reset() override;
    float getBaseline() const override;
    void setBaseline(float baseline) override;
    bool isActive() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm