// Clean Neural Dynamics Header
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed model)
// Implements classic HH equations for neuron membrane dynamics
// Includes voltage-gated sodium and potassium channels with proper gating variable dynamics
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
    void setMembraneCapacitance(float cm);
    float getMembraneCapacitance() const;
    
    void setMaxSodiumConductance(float gNa);
    float getMaxSodiumConductance() const;
    void setMaxPotassiumConductance(float gK);
    float getMaxPotassiumConductance() const;
    void setMaxLeakConductance(float gL);
    float getMaxLeakConductance() const;
    
    void setSodiumReversalPotential(float ENa);
    float getSodiumReversalPotential() const;
    void setPotassiumReversalPotential(float EK);
    float getPotassiumReversalPotential() const;
    void setLeakReversalPotential(float EL);
    float getLeakReversalPotential() const;
    
    // Numerical integration method
    enum class IntegrationMethod { Euler, RungeKutta4 };
    void setIntegrationMethod(IntegrationMethod method);
    IntegrationMethod getIntegrationMethod() const;
    
    // Spike detection and recording
    void setSpikeThreshold(float threshold);
    float getSpikeThreshold() const;
    bool hasRecentSpike() const;
    float getLastSpikeTime() const;
    void clearRecentSpikes();
    
private:
    struct Impl;
    Impl* pImpl;
};

// FitzHugh-Nagumo dynamics (simplified excitable system model)
// Implements fast-slow dynamics with recovery variable
// Supports bistability and limit cycles with different excitability types
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
    void setCubicParameter(float b);
    float getCubicParameter() const;
    
    void setRecoveryTimeConstant(float tau);
    float getRecoveryTimeConstant() const;
    
    void setThreshold(float threshold);
    float getThreshold() const;
    
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
    void setExternalCurrent(float Iext);
    float getExternalCurrent() const;
    
    // Integration method
    enum class IntegrationMethod { Euler, RungeKutta4 };
    void setIntegrationMethod(IntegrationMethod method);
    
    // Excitability type
    enum class ExcitabilityType { TypeI, TypeII };
    void setExcitabilityType(ExcitabilityType type);
    ExcitabilityType getExcitabilityType() const;
    
    // Spike detection
    bool hasRecentSpike() const;
    float getLastSpikeTime() const;
    void clearRecentSpikes();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm