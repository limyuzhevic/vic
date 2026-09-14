#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Abstract base class for neural dynamics models
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one timestep using integrate-and-fire dynamics
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one timestep with realistic synaptic dynamics
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike delivery to postsynaptic neuron
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire based on current membrane potential
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics internal state
    virtual void reset() = 0;
};

// Real biological Integrate-and-Fire dynamics with synaptic currents
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Set membrane time constant (tau_m = RC)
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
    // Set membrane capacitance
    void setMembraneCapacitance(float C);
    float getMembraneCapacitance() const;
    
    // Set synaptic reversal potentials
    void setExcitatoryReversalPotential(float V_e);
    void setInhibitoryReversalPotential(float V_i);
    
    // Enable short-term plasticity
    void enableShortTermPlasticity(bool enable);
    
    // Set synaptic time constants
    void setExcitatorySynapticTimeConstant(float tau_e);
    void setInhibitorySynapticTimeConstant(float tau_i);
    
    // STDP parameters
    void setSTDPParameters(float learningRate, float timeConstant, float traceDecay);
    void setRewardModulation(float strength);
    
    // Spike interval control
    void setMinSpikeInterval(float minInterval);
    void setMaxAdaptationCurrent(float maxCurrent);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed biophysical model)
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // HH-specific parameters
    void setSodiumConductanceMax(float g_Na);
    void setPotassiumConductanceMax(float g_K);
    void setLeakConductance(float g_L);
    void setSodiumReversalPotential(float V_Na);
    void setPotassiumReversalPotential(float V_K);
    void setLeakReversalPotential(float V_L);
    void setTemperature(float temp);
    
private:
    struct Impl;
    Impl* pImpl;
};

// FitzHugh-Nagumo dynamics (simplified excitable system)
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // FHN-specific parameters
    void setRecoveryTimeConstant(float tau_w);
    void setThreshold(float threshold);
    void setResetValue(float resetValue);
    void setReferencePotential(float v_ref);
    void setPositiveSlopeFactor(float a);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
