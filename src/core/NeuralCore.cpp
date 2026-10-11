// NeuralCore implementation - Handles neuron updates, spike detection, and LIF dynamics

#include "NeuralCore.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

NeuralCore::NeuralCore(std::shared_ptr<Brain> brain)
    : brain_(brain), totalSpikes_(0), activeNeurons_(0) {
    // Initialize spike system
    spikeSystem_ = std::make_unique<SpikeSystem>();
    
    // Initialize neural dynamics
    neuralDynamics_ = std::make_unique<IntegrateAndFireDynamics>();
}

NeuralCore::~NeuralCore() = default;

bool NeuralCore::initialize() {
    // Register spike handlers for event-driven processing
    spikeSystem_->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++totalSpikes_;
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem_->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain_->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
    
    return true;
}

void NeuralCore::step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    // Process pending delayed spikes (deliver synaptic input)
    processDelayedSpikes(currentStep, currentTime);
    
    // Update all neurons (LIF dynamics)
    updateAllNeurons(timestep);
    
    // Detect spikes and schedule spike events
    detectAndScheduleSpikes(currentStep, currentTime);
    
    // Process immediate spikes
    spikeSystem_->processSpikes(currentStep);
    
    // Collect statistics
    activeNeurons_ = brain_->getActiveNeuronCount();
}

void NeuralCore::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    spikeSystem_->processDelayedSpikes(currentStep, currentTime);
}

void NeuralCore::updateAllNeurons(TimestepDuration timestep) {
    // Update all neurons using LIF dynamics
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Apply neural dynamics
                neuralDynamics_->updateNeuron(neuron, timestep);
            }
        }
    }
}

void NeuralCore::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->getRegions()) {
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
                    spikeSystem_->queueSpike(event);

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

                        spikeSystem_->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
}

void NeuralCore::recordPlasticityData(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Update synapse state (called during plasticity phase)
            syn->step(currentTime);
        }
    }
}

} // namespace nlm
