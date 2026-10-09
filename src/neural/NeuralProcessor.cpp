#include "NeuralProcessor.hpp"

namespace nlm {

NeuralProcessor::NeuralProcessor(Brain* brain, SpikeSystem* spikeSystem) 
    : brain(brain), spikeSystem(spikeSystem) {
}

void NeuralProcessor::updateNeuronalDynamics(TimestepDuration dt) {
    // Update all neurons with LIF dynamics
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(0.0, dt); // Using current time from step()
            }
        }
    }
}

void NeuralProcessor::processSpikes() {
    spikeSystem->processSpikes(0); // Using current step from step()
}

void NeuralProcessor::processDelayedSpikes(Timestamp currentTime) {
    spikeSystem->processDelayedSpikes(0, currentTime); // Using current step
}

void NeuralProcessor::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain->pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired - queue spike event
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for plasticity
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Schedule delayed spike for each outgoing synapse
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * brain->pImpl->timestep;

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
                }
            }
        }
    }
}

void NeuralProcessor::deliverSynapticInput(const DelayedSpikeEvent& event) {
    // Find destination neuron and deliver synaptic input
    for (auto& region : brain->pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == event.destination_neuron) {
                // Apply synaptic weight as current
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
                return;
            }
        }
    }
}

void NeuralProcessor::updatePlasticityEligibility(SimulationStep currentStep, Timestamp currentTime) {
    // Update synapse state for plasticity calculations
    for (auto& region : brain->pImpl->regions) {
        for (auto* syn : region->getSynapses()) {
            syn->step(currentTime);
        }
    }
}

std::vector<Neuron*> NeuralProcessor::getAllNeurons() const {
    std::vector<Neuron*> neurons;
    for (auto& region : brain->pImpl->regions) {
        auto regionNeurons = region->getAllNeurons();
        neurons.insert(neurons.end(), regionNeurons.begin(), regionNeurons.end());
    }
    return neurons;
}

std::vector<Synapse*> NeuralProcessor::getAllSynapses() const {
    std::vector<Synapse*> synapses;
    for (auto& region : brain->pImpl->regions) {
        auto regionSynapses = region->getSynapses();
        synapses.insert(synapses.end(), regionSynapses.begin(), regionSynapses.end());
    }
    return synapses;
}

void NeuralProcessor::processSpikes(const std::vector<SpikeEvent>& spikeEvents) {
    spikeSystem->processSpikes(spikeEvents);
}

void NeuralProcessor::queueDelayedSpike(const SpikeEvent& spikeEvent, 
                                       const Synapse* synapse, 
                                       Timestamp currentTime) {
    DelayedSpikeEvent delayedEvent(
        spikeEvent.source_neuron,
        synapse->getDestinationNeuron(),
        synapse->getId(),
        synapse->getWeight(),
        synapse->getType(),
        spikeEvent.timestamp,
        currentTime + synapse->getDelay() * brain->pImpl->timestep,
        spikeEvent.step,
        spikeEvent.step + synapse->getDelay()
    );
    spikeSystem->queueDelayedSpike(delayedEvent);
}

} // namespace nlm