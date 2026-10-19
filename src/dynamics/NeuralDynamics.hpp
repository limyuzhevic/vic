#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for neural dynamics
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

// Integrate-and-fire dynamics (simple model)
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
    float getMembraneTimeConstant() const;
    
    // Compatible parameter configuration methods
    void setMembraneCapacitance(float C) { setMembraneTimeConstant(C); }
    float getMembraneCapacitance() const { return getMembraneTimeConstant(); }
    void setRestingPotential(float V_rest) { neuron_->setRestingPotential(V_rest); }
    void setThreshold(float threshold) { neuron_->setThreshold(threshold); }
    void setResetPotential(float V_reset) { neuron_->setResetPotential(V_reset); }
    
private:
    struct Impl;
    Impl* pImpl;
    Neuron* neuron_; // For compatibility (deprecated)
};

// Hodgkin-Huxley dynamics (detailed model)
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override = default;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // HH parameter configuration methods
    void setMembraneCapacitance(float C_m);
    void setMaxSodiumConductance(float g_Na);
    void setMaxPotassiumConductance(float g_K);
    void setLeakConductance(float g_L);
    void setSodiumReversalPotential(float V_Na);
    void setPotassiumReversalPotential(float V_K);
    void setLeakReversalPotential(float V_L);
    void setSodiumActivationTimeConstant(float tau_m);
    void setSodiumInactivationTimeConstant(float tau_h);
    void setPotassiumActivationTimeConstant(float tau_n);
    void setSynapticTimeConstant(float tau_s);
    
    // Get parameter values
    float getMembraneCapacitance() const;
    float getMaxSodiumConductance() const;
    float getMaxPotassiumConductance() const;
    float getLeakConductance() const;
    float getSodiumReversalPotential() const;
    float getPotassiumReversalPotential() const;
    float getLeakReversalPotential() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// FitzHugh-Nagumo dynamics (simplified excitation model)
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override = default;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // FHN parameter configuration methods
    void setVoltageReset(float w_reset);
    void setRecoveryTimeConstant(float tau);
    void setThreshold(float threshold);
    void setExcitability(float a);
    void setThresholdParameter(float b);
    void setSynapticTimeConstant(float tau_s);
    void setAdaptationRate(float rate);
    
    // Get parameter values
    float getVoltageReset() const;
    float getRecoveryTimeConstant() const;
    float getThreshold() const;
    float getExcitability() const;
    float getThresholdParameter() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
