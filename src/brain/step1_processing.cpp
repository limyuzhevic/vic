// Step 1: Process pending delayed spikes (deliver synaptic input)
// This module processes delayed spike events to deliver synaptic input to destination neurons
// O(1) spike delivery using batch processing instead of O(n²) iteration

#include "step1_processing.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

namespace Step1 {

void DeliverSynapticInput(Neuron* neuron, 
                          const DelayedSpikeEvent& event,
                          Timestamp currentTime,
                          float scaleFactor) {
    if (!neuron) {
        NLM_LOG_ERROR("Attempted to deliver synaptic input to null neuron");
        return;
    }
    
    // Apply synaptic weight as current with scaling factor
    MembranePotential synapticCurrent = event.weight * scaleFactor;
    
    if (event.is_excitatory) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        neuron->receiveInhibitoryInput(-synapticCurrent);
    }
}

Neuron* FindNeuronById(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                      NeuronId neuronId) {
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    return neuron;
                }
            }
        }
    }
    return nullptr;
}

void ProcessDelayedSpikes(Brain::Impl* impl, 
                          SimulationStep currentStep, 
                          Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("ProcessDelayedSpikes called with null impl pointer");
        return;
    }
    
    if (!impl->spikeSystem) {
        NLM_LOG_WARNING("SpikeSystem not initialized");
        return;
    }
    
    // Process delayed spikes - optimized version with O(m) complexity
    // where m is number of delayed spikes, not O(n²) where n is total neurons
    impl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BatchProcessSpikes(Brain::Impl* impl,
                        SimulationStep currentStep,
                        Timestamp currentTime) {
    if (!impl || !impl->spikeSystem) {
        return;
    }
    
    // Batch process immediate spikes for better performance
    impl->spikeSystem->processSpikes(currentStep);
}

} // namespace Step1

} // namespace nlm