#include "brain/BrainStep.hpp"
#include "brain/Brain.hpp"

namespace nlm {

void BrainStep::processNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                              TimestepDuration timestep,
                              Timestamp currentTime,
                              size_t& totalSpikesThisStep,
                              SpikeSystem* spikeSystem,
                              std::vector<Neuron*>& sensoryNeurons,
                              NeuralWorkingMemory* workingMemory) {
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : regions) {
        if (!region) continue;
        
        for (auto& population : region->getPopulations()) {
            if (!population) continue;
            
            for (auto* neuron : population->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : regions) {
        if (!region) continue;
        
        for (auto& population : region->getPopulations()) {
            if (!population) continue;
            
            for (auto* neuron : population->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, totalSpikesThisStep);
                    spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        if (syn) {
                            syn->recordPostSpike(currentTime);
                        }
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        if (!syn) continue;
                        
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = totalSpikesThisStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            totalSpikesThisStep,
                            deliveryStep
                        );

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (workingMemory) {
                        float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
                        workingMemory->storeToNeuron(neuron->getId(), activation);
                    }
                }
            }
        }
    }
}

}