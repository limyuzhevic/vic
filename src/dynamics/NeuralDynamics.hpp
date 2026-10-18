#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for neural dynamics
// PHASE 6: Advanced neural dynamics models with biological realism

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    // PHASE 6: Implements detailed neuronal dynamics including ion channels, synaptic integration, and plasticity
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    // PHASE 6: Implements realistic synaptic transmission with short-term and long-term plasticity
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike
    // PHASE 6: Implements conductance-based synaptic currents with neurotransmitter dynamics
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire
    // PHASE 6: Implements threshold detection with refractory period and adaptation
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state
    virtual void reset() = 0;
};

// PHASE 6: Integrate-and-fire dynamics (simple but biophysically plausible model)
// Implements LIF neuron with stochastic channel noise and synaptic adaptation
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
    
    // Additional biophysical parameters
    void setMembraneResistance(float R);
    float getMembraneResistance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PHASE 6: Hodgkin-Huxley dynamics (detailed biophysical model)
// Implements the classic HH equations with sodium, potassium, and leak channels
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
    void setSodiumConductance(float gNa);
    float getSodiumConductance() const;
    void setPotassiumConductance(float gK);
    float getPotassiumConductance() const;
    void setLeakConductance(float gL);
    float getLeakConductance() const;
    void setMembraneCapacitance(float Cm);
    float getMembraneCapacitance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double currentTime;  // Add currentTime member variable
};

// PHASE 6: FitzHugh-Nagumo dynamics (simplified excitable system)
// Implements pattern generation and oscillatory behavior with bistability
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
    void setExcitationParameter(float a);
    float getExcitationParameter() const;
    void setRecoveryParameter(float b);
    float getRecoveryParameter() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double currentTime;  // Add currentTime member variable
};

// Factory function to create appropriate dynamics model based on configuration
std::unique_ptr<NeuralDynamics> createNeuralDynamics(const std::string& modelType = "integrate_and_fire");

} // namespace nlm