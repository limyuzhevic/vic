#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <vector>

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
    void setMembraneResistance(float R);
    float getMembraneResistance() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hodgkin-Huxley dynamics (detailed model)
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// FitzHugh-Nagumo dynamics (simplified excitation model)
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
    void setFHNThreshold(float vThreshold);
    float getFHNThreshold() const;
    void setFHNRestingPotential(float vRest);
    float getFHNRestingPotential() const;
    void setFHNCurrentDegradation(float tau);
    float getFHNCurrentDegradation() const;
    void setFHNActivationThreshold(float aThreshold);
    float getFHNActivationThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
