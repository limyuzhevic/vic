#pragma once

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <vector>

namespace nlm {

class PlasticityManager {
public:
    PlasticityManager(STDP* stdp, Hebbian* hebbian, StructuralPlasticity* structuralPlasticity);
    
    // Apply Spike-Timing-Dependent Plasticity
    void applySTDP(SimulationStep currentStep, Timestamp currentTime);
    
    // Apply Hebbian learning rules
    void applyHebbianLearning(SimulationStep currentStep, Timestamp currentTime);
    
    // Apply structural plasticity (synaptogenesis/pruning)
    void applyStructuralPlasticity(SimulationStep currentStep, const RandomGenerator& rng);
    
    // Integrate neuromodulation effects on plasticity
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Update synaptic weights with neuromodulation scaling
    void updateWeightsWithNeuromodulation(const Neuromodulator& signal);
    
    // Calculate neuromodulation factor for plasticity
    float getPlasticityModulationFactor(const Neuromodulator& signal) const;
    
    // Enable/disable specific plasticity mechanisms
    void enableSTDP(bool enable);
    void enableHebbian(bool enable);
    void enableStructural(bool enable);
    
    // Get current plasticity statistics
    float getAverageSynapticWeight() const;
    size_t getTotalSynapses() const;
    size_t getPlasticityEnabledSynapses() const;
    
private:
    STDP* stdp;
    Hebbian* hebbian;
    StructuralPlasticity* structuralPlasticity;
    
    // Neuromodulation effects
    Neuromodulator currentNeuromodulation;
    bool neuromodulationActive;
};
