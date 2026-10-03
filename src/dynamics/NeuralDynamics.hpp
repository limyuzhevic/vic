#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for neural dynamics
// PLACEHOLDER - Phase 2 will implement various dynamics models

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    // TODO PHASE 2: Implement real dynamics
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    // TODO PHASE 2: Implement real synaptic dynamics
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
    void setMembraneResistance(float R);
    float getMembraneResistance() const;
    void setMembraneCapacitance(float C);
    float getMembraneCapacitance() const;
    void setRestingPotential(float V_rest);
    float getRestingPotential() const;
    void setThreshold(float threshold);
    float getThreshold() const;
    void setResetPotential(float V_reset);
    float getResetPotential() const;
    void setAdaptationTimeConstant(float tau_adapt);
    float getAdaptationTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed model)
// TODO PHASE 2: Implement Hodgkin-Huxley
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    // PLACEHOLDER - Phase 2 will implement HH dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {}
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {}
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {}
    bool shouldFire(const Neuron* neuron) const override { return false; }
    void reset() override {}
    
    // HH parameters
    void setMembraneConductance(float g_Na, float g_K, float g_L);
    void setReversalPotentials(float E_Na, float E_K, float E_L);
    void setCapacitance(float C_m);
    void setRestingPotential(float V_rest);
};

// FitzHugh-Nagumo dynamics (simplified excitation model)
// TODO PHASE 2: Implement FHN dynamics
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    // PLACEHOLDER - Phase 2 will implement FHN dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {}
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {}
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {}
    bool shouldFire(const Neuron* neuron) const override { return false; }
    void reset() override {}
    
    // FHN parameters
    void setFHNParameters(float a, float b, float c);
    void setRestingPotential(float V_rest);
    void setThreshold(float threshold);
};

} // namespace nlm
