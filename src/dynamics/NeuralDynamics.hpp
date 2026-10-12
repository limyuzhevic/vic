#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for neural dynamics
// Real neural dynamics models for Phase 2

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    // Real integrate-and-fire dynamics with biological realism
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    // Real synaptic dynamics with conductance-based models
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike
    // Realistic synaptic transmission with delays and short-term plasticity
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire
    // Includes refractory period and threshold checking
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state to initial conditions
    virtual void reset() = 0;
    
    // Add noise and stochastic components
    // Biological noise in ion channels and synaptic transmission
    virtual void addNoise(Neuron* neuron, Synapse* synapse, RandomGenerator& rng) = 0;
    
    // Calculate synaptic current using conductance-based models
    // Exponential and alpha synaptic conductances
    virtual void calculateSynapticCurrent(Neuron* neuron) const = 0;
};

// Real Integrate-and-Fire dynamics (Phase 2 complete implementation)
// Biological realistic model with conductance-based synapses, noise, and refractory dynamics
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Add noise and stochastic components for biological realism
    void addNoise(Neuron* neuron, Synapse* synapse, RandomGenerator& rng) override;
    
    // Calculate synaptic current using conductance-based models
    // Support both exponential and alpha synaptic conductances
    void calculateSynapticCurrent(Neuron* neuron) const override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    void setMembraneCapacitance(float C);
    float getMembraneCapacitance() const;
    void setThreshold(float threshold);
    float getThreshold() const;
    void setRestingPotential(float V_rest);
    float getRestingPotential() const;
    void setResetPotential(float V_reset);
    float getResetPotential() const;
    void setRefractoryPeriod(uint32_t steps);
    uint32_t getRefractoryPeriod() const;
    
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
};

// FitzHugh-Nagumo dynamics (simplified兴奋性模型)
// TODO PHASE 2: Implement FHN dynamics
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    // PLACEHOLDER - Phase 2 will implement FHN dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {}
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {}
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {}
    bool shouldFire(const Neuron* neuron) const override { return false; }
    void reset() override {}
};

} // namespace nlm
