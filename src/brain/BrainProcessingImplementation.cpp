// Brain Processing Pipeline Implementation
// This file demonstrates improved modularity using the processing pipeline pattern
// Each processing step is a separate method with a single responsibility

#include "BrainProcessingPipeline.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {
namespace brain_processing {

// Processing step result helper functions
inline ProcessingStepResult createStepResult(
    const std::string& name,
    std::chrono::microseconds duration,
    bool success,
    const std::string& message = ""
) {
    return {name, duration, success, message};
}

inline ProcessingStepResult createSuccessStepResult(
    const std::string& name,
    std::chrono::microseconds duration,
    const std::string& message = ""
) {
    return {name, duration, true, message};
}

inline ProcessingStepResult createFailureStepResult(
    const std::string& name,
    std::chrono::microseconds duration,
    const std::string& message
) {
    return {name, duration, false, message};
}

}} // namespace nlm::brain_processing

namespace nlm {

// DefaultBrainProcessingPipeline implementation
DefaultBrainProcessingPipeline::DefaultBrainProcessingPipeline() {
    initializeProcessingSteps();
}

// Execute processing steps using the pipeline
std::vector<ProcessingStepResult> BrainProcessingPipeline::executePipeline(
    SimulationStep currentStep, 
    Timestamp currentTime, 
    Brain* brain
) {
    std::vector<ProcessingStepResult> results;
    
    for (const auto& stepName : getProcessingStepOrder()) {
        auto it = stepFunctions_.find(stepName);
        if (it != stepFunctions_.end()) {
            auto startTime = std::chrono::high_resolution_clock::now();
            auto result = it->second(currentStep, currentTime, brain);
            auto endTime = std::chrono::high_resolution_clock::now();
            result.duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            results.push_back(result);
        }
    }
    
    return results;
}

// Helper method for processing spike events
ProcessingStepResult Brain::processDelayedSpikeEvents(
    SimulationStep currentStep, 
    Timestamp currentTime
) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return {"ProcessDelayedSpikeEvents", duration, true, "Successfully processed delayed spikes"};
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return {"ProcessDelayedSpikeEvents", duration, false, std::string("Failed: ") + e.what()};
    }
}

// Helper method for updating neurons
ProcessingStepResult Brain::updateNeurons(
    SimulationStep currentStep, 
    Timestamp currentTime
) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, pImpl->timestep);
                }
            }
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return {"UpdateNeurons", duration, true, "Successfully updated neurons"};
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return {"UpdateNeurons", duration, false, std::string("Failed: ") + e.what()};
    }
}

// Helper method for detecting spikes
ProcessingStepResult Brain::detectSpikes(
    SimulationStep currentStep, 
    Timestamp currentTime
) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Check if neuron just fired this step
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                    if (justFired) {
                        // Neuron fired this step - queue the spike
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        pImpl->spikeSystem->queueSpike(event);

                        // Record post-synaptic spike for incoming synapses (plasticity)
                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            syn->recordPostSpike(currentTime);
                        }

                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            // Create delayed spike event
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = currentStep + delay;
                            Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

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

                            pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                            // Record pre-synaptic spike for plasticity
                            syn->recordPreSpike(currentTime);
                        }
                        
                        // Store to working memory - neurons that fire become part of working memory
                        if (pImpl->workingMemory) {
                            pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
        
        // Process immediate spikes
        pImpl->spikeSystem->processSpikes(currentStep);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return {"DetectSpikes", duration, true, "Successfully detected and processed spikes"};
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return {"DetectSpikes", duration, false, std::string("Failed: ") + e.what()};
    }
}

// Additional helper methods for other processing steps
// (These follow the same pattern as above)

}} // namespace nlm