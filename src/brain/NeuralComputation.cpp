#include "Integration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuralComputation::initialize(Brain* brainPtr) {
    brain = brainPtr;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
}

void NeuralComputation::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime,
                                            const std::vector<NeuralRegion*>& regions) {
    if (!brain || !brain->getSpikeSystem()) return;
    
    // Process pending delayed spike events (deliver synaptic input)
    brain->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
}

void NeuralComputation::updateNeurons(const std::vector<NeuralRegion*>& regions,
                                     float timestep, Timestamp currentTime) {
    // Step 2: Update all neurons (LIF dynamics)
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void NeuralComputation::detectSpikes(std::vector<NeuralRegion*>& regions,
                                     SpikeSystem* spikeSystem, float timestep,
                                     Timestamp currentTime, SimulationStep currentStep) {
    if (!spikeSystem) return;
    
    // Step 3: Detect spikes and schedule spike events
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * timestep;

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

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (brain && brain->getWorkingMemory()) {
                        brain->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    ++totalSpikesThisStep;
}

void NeuralComputation::processSpikes(SpikeSystem* spikeSystem, SimulationStep currentStep) {
    if (!spikeSystem) return;
    
    // Process immediate spikes
    spikeSystem->processSpikes(currentStep);
}

// Get neural computation statistics
    size_t NeuralComputation::getTotalSpikesThisStep() const {
        return totalSpikesThisStep;
    }
    
    size_t NeuralComputation::getTotalSpikesTotal() const {
        return totalSpikesTotal;
    }
    
} // namespace nlm