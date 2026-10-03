// Step 4: Update working memory
// This module updates working memory with current neural activity patterns
// Implements memory maintenance, competition, and stabilization

#include "step4_update_memory.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step4 {

void CaptureNeuralActivityForWorkingMemory(Brain::Impl* impl,
                                           std::vector<NeuronId>& activeNeurons,
                                           std::vector<float>& activations) {
    if (!impl) {
        return;
    }
    
    activeNeurons.clear();
    activations.clear();
    
    // Capture neurons that are firing or have high membrane potential
    for (auto& region : impl->regions) {
        if (!region) {
            NLM_LOG_WARNING("Null region encountered during working memory capture");
            continue;
        }
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_WARNING("Null population encountered during working memory capture");
                continue;
            }
            
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) {
                    continue;
                }
                
                if (neuron->isFiring()) {
                    activeNeurons.push_back(neuron->getId());
                    activations.push_back(1.0f); // Firing neurons get max activation
                } else {
                    const auto& state = neuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential);
                    if (activation > 5.0f) { // Threshold for significant activity
                        activeNeurons.push_back(neuron->getId());
                        activations.push_back(activation / 20.0f); // Normalize
                    }
                }
            }
        }
    }
}

void UpdateWorkingMemoryTraces(NeuralWorkingMemory* workingMemory,
                               const std::vector<NeuronId>& activeNeurons,
                               const std::vector<float>& activations,
                               TimestepDuration timestep) {
    if (!workingMemory) {
        return;
    }
    
    for (size_t i = 0; i < activeNeurons.size(); ++i) {
        workingMemory->storeToNeuron(activeNeurons[i], activations[i]);
    }
    
    workingMemory->update(timestep);
}

void UpdateWorkingMemory(Brain::Impl* impl,
                         SimulationStep currentStep,
                         Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("UpdateWorkingMemory called with null impl pointer");
        return;
    }
    
    if (!impl->workingMemory) {
        NLM_LOG_DEBUG("Working memory not initialized, skipping update");
        return;
    }
    
    NLM_LOG_DEBUG("Updating working memory (Step " + std::to_string(currentStep) + ")");
    
    std::vector<NeuronId> activeNeurons;
    std::vector<float> activations;
    
    CaptureNeuralActivityForWorkingMemory(impl, activeNeurons, activations);
    UpdateWorkingMemoryTraces(impl->workingMemory, activeNeurons, activations, impl->timestep);
}

void OptimizedWorkingMemoryUpdate(Brain::Impl* impl,
                                  SimulationStep currentStep,
                                  Timestamp currentTime) {
    if (!impl) {
        return;
    }
    
    if (!impl->workingMemory) {
        return;
    }
    
    // Working memory update is already optimized in the main function
    UpdateWorkingMemory(impl, currentStep, currentTime);
}

} // namespace Step4

} // namespace nlm