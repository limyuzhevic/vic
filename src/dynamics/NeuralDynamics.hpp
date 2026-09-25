#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Abstract base class for neural dynamics
// Now implements various biologically accurate dynamics models

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state
    virtual void reset() = 0;
};

// Integrate-and-fire dynamics (LIF model)
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    void setMembraneCapacitance(float capacitance);
    void setRestingPotential(float potential);
    void setThreshold(float threshold);
    void setResetPotential(float potential);
    void setAdaptationConductance(float g_adapt);
    void setRefractoryPeriod(uint32_t steps);
    void setSynapticRiseTime(float riseTime);
    void setSynapticDecayTime(float decayTime);
    
    // Getters
    float getMembraneTimeConstant() const;
    float getMembraneCapacitance() const;
    float getRestingPotential() const;
    float getThreshold() const;
    float getResetPotential() const;
    float getAdaptationConductance() const;
    uint32_t getRefractoryPeriod() const;
    
    // Synaptic plasticity parameters
    void setSTDPLearningRate(float rate);
    void setRewardModulatedLearningRate(float rate);
    void setHebbianLearningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed ion channel model)
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // HH specific parameters
    void setSodiumConductance(float g_Na);
    void setPotassiumConductance(float g_K);
    void setLeakConductance(float g_L);
    void setSodiumReversalPotential(float E_Na);
    void setPotassiumReversalPotential(float E_K);
    void setLeakReversalPotential(float E_L);
    void setTemperature(float temperature);
    
    // Gating variable time constants and steady-state values
    void setAlphaV0(float alphaV0);
    void setBetaV0(float betaV0);
    
    // Synaptic conductance model parameters
    void setMaxSynapticConductance(float g_max);
    void setSynapticTimeConstant(float tau_syn);
    
    // Plasticity parameters
    void setCalciumThreshold(float calciumThreshold);
    void setCalciumDecayRate(float calciumDecayRate);
    
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
    
    // FHN parameters
    void setFHN_a(float a);
    void setFHN_b(float b);
    void setFHN_c(float c);
    void setFHN_tau(float tau);
    void setFHN_epsilon(float epsilon);
    void setFHN_threshold(float threshold);
    
    // Recovery variable parameters
    void setRecoveryTimeConstant(float tau_w);
    void setRecoverySlope(float beta_w);
    
    // Synaptic coupling parameters
    void setCouplingStrength(float coupling);
    void setFHN_resetPotential(float reset); // For spike reset
    
private:
    struct Impl;
    Impl* pImpl;
};

// Voltage-gated channel dynamics (generic implementation)
class VoltageGatedChannelDynamics {
public:
    struct ChannelParameters {
        float conductance;      // Maximum conductance (nS)
        float reversalPotential; // Reversal potential (mV)
        float activationV0;    // Half-activation voltage (mV)
        float activationSlope; // Activation slope (mV)
        float inactivationV0;  // Half-inactivation voltage (mV)
        float inactivationSlope; // Inactivation slope (mV)
        float timeConstant;    // Time constant (ms)
        float steadyStateTau;  // Steady-state time constant (ms)
    };
    
    // Compute activation/inactivation gating variables
    static float computeSteadyStateActivation(float V, const ChannelParameters& params);
    static float computeSteadyStateInactivation(float V, const ChannelParameters& params);
    static float computeTimeConstant(float V, const ChannelParameters& params);
    static float computeGatingVariableDynamics(float currentG, float targetG, float tau, float dt);
    
    // Compute current through channel
    static float computeChannelCurrent(float V, float activation, float inactivation, const ChannelParameters& params);
    
    // Set up standard ion channel parameters
    static ChannelParameters getSodiumChannelParameters();
    static ChannelParameters getPotassiumChannelParameters();
    static ChannelParameters getLeakChannelParameters();
    static ChannelParameters getCalciumChannelParameters();
};

} // namespace nlm

// Integrate-and-fire dynamics (LIF model)
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    void setMembraneCapacitance(float capacitance);
    void setRestingPotential(float potential);
    void setThreshold(float threshold);
    void setResetPotential(float potential);
    void setAdaptationConductance(float g_adapt);
    void setRefractoryPeriod(uint32_t steps);
    void setSynapticRiseTime(float riseTime);
    void setSynapticDecayTime(float decayTime);
    
    // Getters
    float getMembraneTimeConstant() const;
    float getMembraneCapacitance() const;
    float getRestingPotential() const;
    float getThreshold() const;
    float getResetPotential() const;
    float getAdaptationConductance() const;
    uint32_t getRefractoryPeriod() const;
    
    // Synaptic plasticity parameters
    void setSTDPLearningRate(float rate);
    void setRewardModulatedLearningRate(float rate);
    void setHebbianLearningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed ion channel model)
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // HH specific parameters
    void setSodiumConductance(float g_Na);
    void setPotassiumConductance(float g_K);
    void setLeakConductance(float g_L);
    void setSodiumReversalPotential(float E_Na);
    void setPotassiumReversalPotential(float E_K);
    void setLeakReversalPotential(float E_L);
    void setTemperature(float temperature);
    
    // Gating variable time constants and steady-state values
    void setAlphaV0(float alphaV0);
    void setBetaV0(float betaV0);
    
    // Synaptic conductance model parameters
    void setMaxSynapticConductance(float g_max);
    void setSynapticTimeConstant(float tau_syn);
    
    // Plasticity parameters
    void setCalciumThreshold(float calciumThreshold);
    void setCalciumDecayRate(float calciumDecayRate);
    
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
    
    // FHN parameters
    void setFHN_a(float a);
    void setFHN_b(float b);
    void setFHN_c(float c);
    void setFHN_tau(float tau);
    void setFHN_epsilon(float epsilon);
    void setFHN_threshold(float threshold);
    
    // Recovery variable parameters
    void setRecoveryTimeConstant(float tau_w);
    void setRecoverySlope(float beta_w);
    
    // Synaptic coupling parameters
    void setCouplingStrength(float coupling);
    void setFHN_resetPotential(float reset); // For spike reset
    
private:
    struct Impl;
    Impl* pImpl;
};

// Voltage-gated channel dynamics (generic implementation)
class VoltageGatedChannelDynamics {
public:
    struct ChannelParameters {
        float conductance;      // Maximum conductance (nS)
        float reversalPotential; // Reversal potential (mV)
        float activationV0;    // Half-activation voltage (mV)
        float activationSlope; // Activation slope (mV)
        float inactivationV0;  // Half-inactivation voltage (mV)
        float inactivationSlope; // Inactivation slope (mV)
        float timeConstant;    // Time constant (ms)
        float steadyStateTau;  // Steady-state time constant (ms)
    };
    
    // Compute activation/inactivation gating variables
    static float computeSteadyStateActivation(float V, const ChannelParameters& params);
    static float computeSteadyStateInactivation(float V, const ChannelParameters& params);
    static float computeTimeConstant(float V, const ChannelParameters& params);
    static float computeGatingVariableDynamics(float currentG, float targetG, float tau, float dt);
    
    // Compute current through channel
    static float computeChannelCurrent(float V, float activation, float inactivation, const ChannelParameters& params);
    
    // Set up standard ion channel parameters
    static ChannelParameters getSodiumChannelParameters();
    static ChannelParameters getPotassiumChannelParameters();
    static ChannelParameters getLeakChannelParameters();
    static ChannelParameters getCalciumChannelParameters();
};

} // namespace nlm

} // namespace nlm
