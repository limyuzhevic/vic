#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for neural dynamics
// Provides interface for different neural computation models

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    /**
     * Update neuron state for one simulation step
     * 
     * @param neuron Neuron to update
     * @param dt Simulation timestep (ms)
     */
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    /**
     * Update synapse state for one simulation step
     * 
     * @param synapse Synapse to update
     * @param dt Simulation timestep (ms)
     */
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    /**
     * Apply synaptic input resulting from spike transmission
     * 
     * @param neuron Destination neuron
     * @param synapse Synapse that transmitted the spike
     */
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    /**
     * Check if neuron has reached firing threshold
     * 
     * @param neuron Neuron to check
     * @return true if neuron should fire this step
     */
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    /**
     * Reset dynamics state to initial conditions
     */
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
