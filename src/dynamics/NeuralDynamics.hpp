#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for neural dynamics
// Implements real physics-based neural dynamics for spiking neural networks

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step using real LIF dynamics
    // Implements Leaky Integrate-and-Fire: v(t) = v_reset + (v(t-1) - v_reset) * exp(-dt/tau) + I_syn * tau
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step with real synaptic dynamics
    // Includes short-term plasticity, eligibility traces, and spike propagation
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike with conductance-based dynamics
    // Converts spike into post-synaptic conductance changes
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire based on membrane potential threshold
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state to initial values
    virtual void reset() = 0;
};

// Integrate-and-fire dynamics (standard LIF model)
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    // Update neuron using real LIF exponential dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    
    // Update synapse with short-term plasticity and use-dependent modulation
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    
    // Apply synaptic input from spike using conductance-based model
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    
    // Check if neuron exceeds firing threshold
    bool shouldFire(const Neuron* neuron) const override;
    
    // Reset dynamics internal state
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed biophysical model)
// Models ion channel dynamics for action potential generation
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
};

// FitzHugh-Nagumo dynamics (simplified excitable system model)
// Fast-slow neural dynamics model with recovery variable
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
};

} // namespace nlm
