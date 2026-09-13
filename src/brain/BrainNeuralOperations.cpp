#include "BrainNeuralOperations.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void BrainNeuralOperations::processSpikes(SimulationStep currentStep) {
    // Process immediate spike events (if any)
    // This would typically be implemented in the SpikeSystem
}

void BrainNeuralOperations::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Process pending delayed spike events to deliver synaptic input
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == event.destination_neuron) {
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        }
    }
}

void BrainNeuralOperations::updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
    // Update all neuron membrane potentials (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void BrainNeuralOperations::detectSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Detect spikes and schedule spike events
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    queueSpike(event);

                    // Record post-synaptic spike for plasticity
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Schedule delayed spike events for outgoing synapses
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
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

                        queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Update working memory
                    if (pImpl->workingMemory) {
                        updateWorkingMemory(neuron, std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainNeuralOperations::queueSpike(const SpikeEvent& event) {
    // Queue spike for immediate processing
    pImpl->spikeSystem->queueSpike(event);
    totalSpikesThisStep++;
    totalSpikesTotal++;
}

void BrainNeuralOperations::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Queue delayed spike for future processing
    pImpl->spikeSystem->queueDelayedSpike(event);
}

void BrainNeuralOperations::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainNeuralOperations::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

void BrainNeuralOperations::receiveSensoryInput(const class SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;

    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;

    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }

        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);

        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            updateWorkingMemory(pImpl->sensoryNeurons[i], normalizedValue / 10.0f);
        }
    }
}

std::unique_ptr<class Action> BrainNeuralOperations::produceAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }

    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }

    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }

    return std::make_unique<Action>(type);
}

size_t BrainNeuralOperations::getTotalSpikeCount() const {
    return totalSpikesTotal;
}

size_t BrainNeuralOperations::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

void BrainNeuralOperations::deliverSynapticInput(const DelayedSpikeEvent& event) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == event.destination_neuron) {
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        }
    }
}

void BrainNeuralOperations::updateWorkingMemory(const Neuron* neuron, float activation) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeToNeuron(neuron->getId(), activation);
    }
}

void BrainNeuralOperations::scheduleSpikes(const std::vector<SpikeEvent>& events) {
    for (const auto& event : events) {
        queueSpike(event);
    }
}

void BrainNeuralOperations::updatePlasticity(float plasticityMod) {
    // This function would be implemented in the plasticity system
    // Here we just provide the interface
}

} // namespace nlm