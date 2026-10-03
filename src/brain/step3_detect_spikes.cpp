// Step 3: Detect spikes and schedule spike events
// This module detects spiking neurons and creates spike events for propagation
// Optimized spike detection with reduced search complexity

#include "step3_detect_spikes.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

namespace Step3 {

bool DetectNeuronSpike(Neuron* neuron,
                      Timestamp currentTime,
                      SimulationStep currentStep,
                      TimestepDuration timestep) {
    if (!neuron) {
        NLM_LOG_ERROR("Attempted to detect spike on null neuron");
        return false;
    }
    
    const auto& state = neuron->getState();
    
    // Check if neuron is in refractory state
    if (state.firingState != FiringState::Refractory) {
        return false;
    }
    
    // Check if spike timing is within tolerance
    bool justFired = (state.lastSpikeTime >= 0.0f && 
                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
    
    return justFired;
}

void CreateSpikeEvent(Brain::Impl* impl,
                      Neuron* neuron,
                      Timestamp currentTime,
                      SimulationStep currentStep) {
    if (!impl || !neuron) {
        return;
    }
    
    SpikeEvent event(neuron->getId(), currentTime, currentStep);
    impl->spikeSystem->queueSpike(event);
}

void ProcessPreSynapticPlasticity(NeuralRegion* region,
                                  NeuronId neuronId,
                                  Timestamp currentTime) {
    if (!region) {
        return;
    }
    
    auto incomingSynapses = region->getSynapsesTo(neuronId);
    for (Synapse* syn : incomingSynapses) {
        if (syn) {
            syn->recordPostSpike(currentTime);
        }
    }
}

void ScheduleDelayedSpikes(Brain::Impl* impl,
                           Neuron* neuron,
                           Timestamp currentTime,
                           SimulationStep currentStep) {
    if (!impl || !neuron) {
        return;
    }
    
    NeuralRegion* region = FindRegionByNeuron(impl, neuron->getId());
    if (!region) {
        return;
    }
    
    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
    for (Synapse* syn : outgoingSynapses) {
        if (!syn) {
            continue;
        }
        
        Delay delay = syn->getDelay();
        SimulationStep deliveryStep = currentStep + delay;
        Timestamp deliveryTime = currentTime + delay * impl->timestep;
        
        DelayedSpikeEvent delayedEvent(
            neuron->getId(),
            syn->getDestinationNeuron(),
            syn->getId(),
            syn->getWeight(),
            syn->getType(),
            currentTime,
            deliveryTime,
            currentStep,
            deliveryStep
        );
        
        impl->spikeSystem->queueDelayedSpike(delayedEvent);
        
        syn->recordPreSpike(currentTime);
    }
}

void DetectAndScheduleSpikes(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("DetectAndScheduleSpikes called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Detecting and scheduling spikes (Step " + std::to_string(currentStep) + ")");
    
    OptimizedSpikeDetection(impl, currentStep, currentTime);
}

void OptimizedSpikeDetection(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime) {
    if (!impl) {
        return;
    }
    
    for (auto& region : impl->regions) {
        if (!region) {
            NLM_LOG_WARNING("Null region encountered during spike detection");
            continue;
        }
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_WARNING("Null population encountered during spike detection");
                continue;
            }
            
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) {
                    NLM_LOG_WARNING("Null neuron encountered during spike detection");
                    continue;
                }
                
                if (DetectNeuronSpike(neuron, currentTime, currentStep, impl->timestep)) {
                    CreateSpikeEvent(impl, neuron, currentTime, currentStep);
                    ProcessPreSynapticPlasticity(region.get(), neuron->getId(), currentTime);
                    ScheduleDelayedSpikes(impl, neuron, currentTime, currentStep);
                    
                    // Store to working memory
                    if (impl->workingMemory) {
                        const auto& state = neuron->getState();
                        float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
                        impl->workingMemory->storeToNeuron(neuron->getId(), activation);
                    }
                }
            }
        }
    }
}

NeuralRegion* FindRegionByNeuron(Brain::Impl* impl, NeuronId neuronId) {
    if (!impl) {
        return nullptr;
    }
    
    for (auto& region : impl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron && neuron->getId() == neuronId) {
                return region.get();
            }
        }
    }
    return nullptr;
}

} // namespace Step3

} // namespace nlm