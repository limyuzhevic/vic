#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Implements real synaptic plasticity with multiple mechanisms
// Based on biological learning rules with temporal dynamics

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Implements biologically plausible plasticity rules
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change to synapse
    // Implements weight clipping and bounds checking
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name for identification
    virtual const char* getName() const = 0;
    
    // Check if rule is currently enabled
    bool isEnabled() const;
    
    // Enable or disable the plasticity rule
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;  // Whether this plasticity rule is active
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Implements classic Hebbian learning with covariance-based updates
// Based on biological Hebbian mechanisms in hippocampus and cortex
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    // Update synaptic weights using Hebbian covariance rule
    // Implements: Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change with biological constraints
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule identification
    const char* getName() const override;
    
    // Get current learning rate parameter
    float getLearningRate() const;
    
    // Set learning rate with bounds checking
    void setLearningRate(float rate);
    
    // Get synaptic modification threshold (for BCM-like behavior)
    float getSynapticModificationThreshold() const;
    void setSynapticModificationThreshold(float threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated Hebbian rule (dopamine-dependent)
// Combines Hebbian learning with reward prediction error
// Implements reinforcement learning at synaptic level
class RewardModulatedHebbianRule : public PlasticityRule {
public:
    RewardModulatedHebbianRule();
    ~RewardModulatedHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override { return "RewardModulatedHebbian"; }
    
    // Set reward prediction error gain
    void setErrorGain(float gain);
    
    // Set baseline reward level
    void setBaselineReward(float baseline);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// Implements homeostatic plasticity to prevent runaway excitation
// Based on anti-Hebbian learning in biological systems
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override { return "AntiHebbian"; }
    
    // Set depression rate
    void setDepressionRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) synaptic plasticity rule
// Implements sliding threshold for potentiation vs depression
// Based on cortical synaptic learning rules
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override { return "BCM"; }
    
    // Set theta-m variable (sliding modification threshold)
    void setThetaM(float thetaM);
    
    // Set learning rate
    void setLearningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-Timing-Dependent Plasticity (STDP)
// Core mechanism for temporal learning in neural circuits
// Biologically accurate STDP with pre- and post-synaptic interactions
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override { return "STDP"; }
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    void setSoftBounds(bool enabled);   // Enable soft weight bounds
    bool getSoftBounds() const;
    
    // Configure STDP parameters from experiment
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
