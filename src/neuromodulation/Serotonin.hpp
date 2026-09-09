#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

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