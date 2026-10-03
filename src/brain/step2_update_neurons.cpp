// Step 2: Update all neurons (LIF dynamics)
// This module updates all neuron membrane potentials using Leaky Integrate-and-Fire dynamics
// Provides optimized neuron stepping with null pointer validation and error handling

#include "step2_update_neurons.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

namespace Step2 {

bool StepNeuron(Neuron* neuron, 
                Timestamp currentTime, 
                TimestepDuration timestep,
                size_t neuronIndex) {
    if (!neuron) {
        NLM_LOG_ERROR("Attempted to step null neuron at index " + std::to_string(neuronIndex));
        return false;
    }
    
    try {
        neuron->stepLIF(currentTime, timestep);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception stepping neuron at index " + std::to_string(neuronIndex) + ": " + e.what());
        return false;
    }
}

void UpdateNeuronPopulation(const std::vector<Neuron*>& neurons,
                            Timestamp currentTime,
                            TimestepDuration timestep,
                            size_t startIndex,
                            size_t count) {
    size_t endIndex = (count > 0) ? std::min(startIndex + count, neurons.size()) : neurons.size();
    
    for (size_t i = startIndex; i < endIndex; ++i) {
        if (neurons[i]) {
            StepNeuron(neurons[i], currentTime, timestep, i);
        }
    }
}

void UpdateNeurons(Brain::Impl* impl,
                   SimulationStep currentStep,
                   Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("UpdateNeurons called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Updating neuron membrane potentials (Step " + std::to_string(currentStep) + ")");
    
    OptimizedNeuronStepping(impl, currentStep, currentTime);
}

void OptimizedNeuronStepping(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime) {
    if (!impl) {
        return;
    }
    
    // Batch process neurons by region for better cache locality
    for (auto& region : impl->regions) {
        if (!region) {
            NLM_LOG_WARNING("Null region encountered during neuron update");
            continue;
        }
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_WARNING("Null population encountered during neuron update");
                continue;
            }
            
            auto neurons = pop->getNeurons();
            if (neurons.empty()) {
                continue;
            }
            
            // Update all neurons in this population
            UpdateNeuronPopulation(neurons, currentTime, impl->timestep, 0, neurons.size());
        }
    }
}

} // namespace Step2

} // namespace nlm