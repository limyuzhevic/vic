#include "NeuronSystemUpdater.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuronSystemUpdater::update(Brain& brain, SimulationStep currentStep,
                                Timestamp currentTime, TimestepDuration timestep) {
    // Update neuron state and handle spike events
    brain.pImpl->currentStep = currentStep;
    brain.pImpl->currentTime = currentTime;
    brain.pImpl->totalSpikesThisStep = 0;
    
    // Step 1: Process pending delayed spike events (deliver synaptic input)
    processDelayedSpikes(brain, currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    updateNeurons(brain, currentTime, timestep);
    
    // Step 3: Detect spikes and schedule outgoing spike events
    detectSpikes(brain, currentStep, currentTime);
    
    // Process immediate spikes
    processSpikes(brain, currentStep);
}

void NeuronSystemUpdater::processDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    // Find neurons to update
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Handle delayed synaptic input
                neuron->stepLIF(currentTime, brain.pImpl->timestep);
            }
        }
    }
}

void NeuronSystemUpdater::updateNeurons(Brain& brain, Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void NeuronSystemUpdater::detectSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain.pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    brain.pImpl->spikeSystem->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * brain.pImpl->timestep;

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

                        brain.pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (brain.pImpl->workingMemory) {
                        brain.pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void NeuronSystemUpdater::processSpikes(Brain& brain, SimulationStep currentStep) {
    brain.pImpl->spikeSystem->processSpikes(currentStep);
}

} // namespace nlm
