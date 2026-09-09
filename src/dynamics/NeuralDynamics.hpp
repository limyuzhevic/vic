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
    
    // Configuration methods
    void setNeuronType(NeuronType type);
    void setTemperature(float tempCelsius);
    void setAdaptationConductance(float g_adapt);
    void setSynapticTimeConstant(float tau_syn);
    void setRefractoryPeriod(uint32_t abs_ref, uint32_t rel_ref);
    
    // Parameter getter methods
    float getAdaptationConductance() const;
    float getSynapticTimeConstant() const;
    uint32_t getAbsoluteRefractoryPeriod() const;
    uint32_t getRelativeRefractoryPeriod() const;
    
    NeuronType getNeuronType() const;
    float getTemperature() const;

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
