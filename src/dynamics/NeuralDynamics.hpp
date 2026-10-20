#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for neural dynamics
// Enhanced version with comprehensive neural dynamics models

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state for one step
    // Updated to support advanced dynamics
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    // Update synapse state for one step
    // Enhanced to support real synaptic dynamics
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    // Apply synaptic input from spike
    // Enhanced for conductance-based and realistic synaptic input
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    // Check if neuron should fire
    // Enhanced for refractory period handling and adaptation
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    // Reset dynamics state
    virtual void reset() = 0;
};

// Integrate-and-fire dynamics (biologically plausible model)
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Core membrane parameters
    void setMembraneCapacitance(float C);  // nF
    float getMembraneCapacitance() const;
    void setMembraneResistance(float R);   // MOhm
    float getMembraneResistance() const;
    void setMembraneTimeConstant(float tau);  // ms
    float getMembraneTimeConstant() const;
    
    // Membrane potentials
    void setRestingPotential(float V_rest);  // mV
    float getRestingPotential() const;
    void setThresholdPotential(float V_th);  // mV
    float getThresholdPotential() const;
    void setResetPotential(float V_reset);   // mV
    float getResetPotential() const;
    void setLeakConductance(float g_leak);   // nS
    float getLeakConductance() const;
    
    // Refractory period control
    void setRefractoryPeriod(uint32_t steps);      // Absolute refractory period
    uint32_t getRefractoryPeriod() const;
    void setAdaptationPeriod(uint32_t steps);       // Relative refractory period
    uint32_t getAdaptationPeriod() const;
    
    // Adaptation parameters
    void setSpikeAdaptation(float g_Adap);         // Adaptation current conductance (nS)
    float getSpikeAdaptation() const;
    void setAdaptationTimeConstant(float tau_Adap); // ms
    float getAdaptationTimeConstant() const;
    
    // Synaptic parameters
    void setSynapticTimeConstant(float tau_syn);    // ms (for AMPA/NMDA dynamics)
    float getSynapticTimeConstant() const;
    void setGABA_A_TimeConstant(float tau_GABA_A); // ms
    float getGABA_A_TimeConstant() const;
    void setGABA_B_TimeConstant(float tau_GABA_B); // ms
    float getGABA_B_TimeConstant() const;
    
    // Noise and randomness
    void setNoiseLevel(float noise);               // Standard deviation of noise current
    float getNoiseLevel() const;
    void setSTDPLearningRate(float rate);           // Spike timing dependent plasticity rate
    float getSTDPLearningRate() const;
    
    // Short-term plasticity parameters
    void setDepressionTimeConstant(float tau_D);   // ms for synaptic depression
    float getDepressionTimeConstant() const;
    void setFacilitationTimeConstant(float tau_F); // ms for synaptic facilitation
    float getFacilitationTimeConstant() const;
    
    // Reversal potentials
    void setAMPA_ReversalPotential(float E_AMPA);  // mV
    float getAMPA_ReversalPotential() const;
    void setNMDA_ReversalPotential(float E_NMDA);  // mV
    float getNMDA_ReversalPotential() const;
    void setGABA_A_ReversalPotential(float E_GABA_A); // mV
    float getGABA_A_ReversalPotential() const;
    void setGABA_B_ReversalPotential(float E_GABA_B); // mV
    float getGABA_B_ReversalPotential() const;
    
    // State management
    void setMaxSpikeHistory(size_t max_history);   // Maximum recorded spike timestamps
    size_t getMaxSpikeHistory() const;
    
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
